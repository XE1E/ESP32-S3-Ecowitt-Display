/**
 * ESP32-S3-Ecowitt-Display
 *
 * Display táctil de 7" para estación meteorológica Ecowitt.
 * Muestra datos en tiempo real desde tu propio servidor.
 * Opcionalmente envía datos del BME280 como estación remota.
 *
 * Hardware: Waveshare ESP32-S3-Touch-LCD-7B (1024x600)
 * Framework: LVGL 8.3
 *
 * https://github.com/XE1E/ESP32-S3-Ecowitt-Display
 */

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <Wire.h>
#include <time.h>

#include "config.h"
#include "my_config.h"
#include "ecowitt_api.h"
#include "bme280_sensor.h"
#include "preferences_manager.h"

// LVGL y display
#include "display.h"
#include "touch.h"
#include "ui_dashboard.h"
#include "ui_navigation.h"
#include "ui_wizard.h"

// ============================================================================
// Variables globales
// ============================================================================

WeatherData g_weather = {0};
CompareData g_compare = {0};
AlertData g_alerts = {0};
AlmanacData g_almanac = {0};
SystemStatus g_status = {0};
LocalSensorData g_local = {0};
RemoteStationConfig g_remote_config = {0};

EcowittAPI ecowittApi;
Preferences preferences;

// Timers
unsigned long lastWeatherUpdate = 0;
unsigned long lastBME280Read = 0;
unsigned long lastRemoteSend = 0;
unsigned long lastAlmanacUpdate = 0;

// Intervalos (ms)
const unsigned long WEATHER_INTERVAL = 60000;      // 1 minuto
const unsigned long BME280_INTERVAL = 10000;       // 10 segundos
const unsigned long ALMANAC_INTERVAL = 600000;     // 10 minutos
const unsigned long CLOCK_INTERVAL = 1000;         // 1 segundo

unsigned long lastClockUpdate = 0;

// ============================================================================
// Setup
// ============================================================================

// Flag para saber si wizard está activo
bool wizardActive = false;

// Callback cuando el wizard termina
void onWizardDone(bool success) {
    wizardActive = false;
    if (success) {
        Serial.println("[WIZARD] Completado exitosamente");
        startNormalOperation();
    } else {
        Serial.println("[WIZARD] Cancelado - reiniciando wizard");
        showWizard(onWizardDone);
        wizardActive = true;
    }
}

// Iniciar operación normal (después de wizard o si ya configurado)
void startNormalOperation() {
    // Configurar NTP
    configTzTime(TIMEZONE, "pool.ntp.org", "time.nist.gov");
    Serial.println("[NTP] Sincronizando hora...");

    // Verificar conexión al servidor
    Serial.println("[API] Verificando conexión al servidor...");
    if (ecowittApi.checkConnection()) {
        Serial.println("[API] Servidor OK");
        g_status.api_ok = true;
    } else {
        Serial.println("[API] No se pudo conectar al servidor");
        g_status.api_ok = false;
    }

    // Primera carga de datos
    updateWeatherData();
    updateAlmanac();

    // Crear dashboard
    createDashboard();
    updateDashboardTime();
    updateDashboardWeather();

    // Inicializar navegación touch
    initNavigation();

    Serial.println();
    Serial.println("[SYS] Setup completo!");
    Serial.println("══════════════════════════════════════════════════");
    Serial.println();
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n*** BOOT ***");

    Serial.println("╔═══════════════════════════════════════════════╗");
    Serial.println("║     ESP32-S3 Ecowitt Display                  ║");
    Serial.println("║     github.com/XE1E/ESP32-S3-Ecowitt-Display  ║");
    Serial.println("╚═══════════════════════════════════════════════╝");
    Serial.println();

    // ========================================================================
    // Verificar PSRAM (crítico para display 1024x600)
    // ========================================================================
    if (psramFound()) {
        Serial.printf("[SYS] PSRAM: %.2f MB disponible\n",
                      ESP.getFreePsram() / 1024.0 / 1024.0);
    } else {
        Serial.println("[SYS] ERROR: PSRAM no encontrada!");
        Serial.println("[SYS] Verifica configuración: PSRAM = OPI PSRAM");
        while (1) {
            delay(1000);
        }
    }

    // ========================================================================
    // Cargar preferencias
    // ========================================================================
    setDefaultPreferences();
    bool configured = loadPreferences();
    incrementBootCount();
    Serial.printf("[PREFS] Configurado: %s\n", configured ? "Sí" : "No");

    // ========================================================================
    // Inicializar I2C (compartido por touch y BME280)
    // ========================================================================
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    Wire.setClock(400000);
    Serial.printf("[I2C] Inicializado en GPIO %d/%d\n", TOUCH_SDA, TOUCH_SCL);

    // I2C Scanner para debug
    Serial.println("[I2C] Scanning...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("[I2C] Device found at 0x%02X\n", addr);
        }
    }
    Serial.println("[I2C] Scan complete");

    // ========================================================================
    // Inicializar BME280 (opcional)
    // ========================================================================
#ifdef BME280_ENABLED
    if (initBME280(BME280_ADDRESS)) {
        g_status.bme280_ok = true;
        if (readBME280(g_local)) {
            Serial.printf("[BME280] Inicial: %.1f°C, %.0f%%, %.0f hPa\n",
                          g_local.temperature, g_local.humidity, g_local.pressure);
        }
    } else {
        g_status.bme280_ok = false;
    }
#endif

    // ========================================================================
    // Configurar estación remota (desde preferencias o config)
    // ========================================================================
    UserPreferences* prefs = getPreferences();
    if (prefs->remote_enabled) {
        g_remote_config.enabled = true;
        strncpy(g_remote_config.passkey, prefs->remote_passkey, sizeof(g_remote_config.passkey));
        strncpy(g_remote_config.label, prefs->remote_label, sizeof(g_remote_config.label));
        g_remote_config.send_interval = max(60, (int)prefs->remote_interval);
        Serial.printf("[REMOTE] Estación remota: %s\n", g_remote_config.label);
    } else {
#ifdef REMOTE_STATION_ENABLED
        g_remote_config.enabled = true;
        strncpy(g_remote_config.passkey, REMOTE_STATION_PASSKEY, sizeof(g_remote_config.passkey));
        strncpy(g_remote_config.label, REMOTE_STATION_LABEL, sizeof(g_remote_config.label));
        g_remote_config.send_interval = max(60, REMOTE_STATION_INTERVAL);
#else
        g_remote_config.enabled = false;
#endif
    }

    // ========================================================================
    // Inicializar display y touch
    // ========================================================================
    Serial.println("[INIT] Iniciando display...");
    Serial.flush();
    initDisplay();
    Serial.println("[INIT] Display OK");
    Serial.flush();

    Serial.println("[INIT] Iniciando touch...");
    Serial.flush();
    initTouch();
    Serial.println("[INIT] Touch OK");
    Serial.flush();

    Serial.println("[INIT] Iniciando LVGL...");
    Serial.flush();
    initLVGL();
    Serial.println("[INIT] LVGL OK");
    Serial.flush();

    // ========================================================================
    // Decidir: Wizard o operación normal
    // ========================================================================
    if (!configured) {
        // Primera ejecución - mostrar wizard
        Serial.println("[SYS] Primera ejecución - iniciando wizard");
        showWizard(onWizardDone);
        wizardActive = true;
    } else {
        // Ya configurado - conectar con valores guardados
        connectWiFi();
        startNormalOperation();
    }
}

// ============================================================================
// Loop principal
// ============================================================================

static unsigned long lastLvglTick = 0;

void loop() {
    unsigned long now = millis();

    // ========================================================================
    // LVGL tick (must be called before lv_timer_handler)
    // ========================================================================
    uint32_t elapsed = now - lastLvglTick;
    if (elapsed > 0) {
        lv_tick_inc(elapsed);
        lastLvglTick = now;
    }

    // ========================================================================
    // Procesar LVGL (siempre)
    // ========================================================================
    lv_timer_handler();

    // ========================================================================
    // Si wizard activo, solo procesar LVGL
    // ========================================================================
    if (wizardActive) {
        delay(5);
        return;
    }

    // ========================================================================
    // Actualizar reloj cada segundo
    // ========================================================================
    if (now - lastClockUpdate >= CLOCK_INTERVAL) {
        lastClockUpdate = now;
        updateDashboardTime();
    }

    // ========================================================================
    // Leer BME280 local
    // ========================================================================
#ifdef BME280_ENABLED
    if (g_status.bme280_ok && (now - lastBME280Read >= BME280_INTERVAL)) {
        lastBME280Read = now;
        if (readBME280(g_local)) {
            g_status.last_bme_read = now;
            Serial.printf("[BME280] %.1f°C  %.0f%%  %.0f hPa\n",
                          g_local.temperature, g_local.humidity, g_local.pressure);
        }
    }
#endif

    // ========================================================================
    // Enviar datos como estación remota
    // ========================================================================
    if (g_remote_config.enabled && g_status.bme280_ok && g_local.valid) {
        unsigned long remoteInterval = g_remote_config.send_interval * 1000UL;
        if (now - lastRemoteSend >= remoteInterval) {
            lastRemoteSend = now;
            bool ok = ecowittApi.postLocalSensorData(
                g_remote_config.passkey,
                g_local.temperature,
                g_local.humidity,
                g_local.pressure
            );
            g_status.remote_station_ok = ok;
            g_status.last_remote_send = now;
        }
    }

    // ========================================================================
    // Actualizar datos del servidor
    // ========================================================================
    if (now - lastWeatherUpdate >= WEATHER_INTERVAL) {
        lastWeatherUpdate = now;
        updateWeatherData();
    }

    // ========================================================================
    // Actualizar almanaque (menos frecuente)
    // ========================================================================
    if (now - lastAlmanacUpdate >= ALMANAC_INTERVAL) {
        lastAlmanacUpdate = now;
        updateAlmanac();
    }

    // ========================================================================
    // Verificar WiFi y reconectar si es necesario
    // ========================================================================
    if (WiFi.status() != WL_CONNECTED) {
        g_status.wifi_connected = false;
        Serial.println("[WiFi] Conexión perdida, reconectando...");
        connectWiFi();
    }

    // Pequeña pausa
    delay(5);
}

// ============================================================================
// Conectar WiFi
// ============================================================================

void connectWiFi() {
    // Usar preferencias si están configuradas, sino usar defines
    const char* ssid;
    const char* pass;

    UserPreferences* prefs = getPreferences();
    if (prefs->configured && strlen(prefs->wifi_ssid) > 0) {
        ssid = prefs->wifi_ssid;
        pass = prefs->wifi_pass;
    } else {
        ssid = WIFI_SSID;
        pass = WIFI_PASSWORD;
    }

    Serial.printf("[WiFi] Conectando a %s", ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" OK!");
        Serial.printf("[WiFi] IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WiFi] RSSI: %d dBm\n", WiFi.RSSI());
        g_status.wifi_connected = true;
        g_status.wifi_rssi = WiFi.RSSI();
    } else {
        Serial.println(" FALLO!");
        g_status.wifi_connected = false;
    }
}

// ============================================================================
// Actualizar datos meteorológicos del servidor
// ============================================================================

void updateWeatherData() {
    Serial.println("[API] Actualizando datos...");

    // Datos actuales
    if (ecowittApi.fetchCurrent(g_weather)) {
        Serial.printf("[API] Exterior: %.1f°C, %.0f%%, %.1f hPa\n",
                      g_weather.temp_outdoor,
                      g_weather.humidity_outdoor,
                      g_weather.pressure_rel);
        Serial.printf("[API] Viento: %.1f km/h %s, Ráfaga: %.1f\n",
                      g_weather.wind_speed,
                      g_weather.wind_dir_cardinal,
                      g_weather.wind_gust);
        Serial.printf("[API] Interior (consola): %.1f°C, %.0f%%\n",
                      g_weather.temp_indoor,
                      g_weather.humidity_indoor);
        g_status.api_ok = true;
    } else {
        Serial.println("[API] ERROR: No se pudieron obtener datos actuales");
        g_status.api_ok = false;
        return;  // No continuar si falla el principal
    }

    // Estadísticas del día
    if (ecowittApi.fetchDailyStats(g_weather)) {
        Serial.printf("[API] Hoy: Máx %.1f°C, Mín %.1f°C, Lluvia %.1f mm\n",
                      g_weather.temp_max,
                      g_weather.temp_min,
                      g_weather.rain_total);
    }

    // Comparación vs ayer
    if (ecowittApi.fetchCompare(g_compare)) {
        Serial.printf("[API] vs ayer: %+.1f°C, %+.0f%% HR, %+.1f hPa\n",
                      g_compare.temp_diff,
                      g_compare.humidity_diff,
                      g_compare.pressure_diff);
    }

    // Alertas
    if (ecowittApi.fetchAlerts(g_alerts)) {
        if (g_alerts.has_alerts) {
            Serial.printf("[API] ⚠️ %d ALERTAS ACTIVAS:\n", g_alerts.alert_count);
            for (int i = 0; i < g_alerts.alert_count; i++) {
                Serial.printf("       - %s\n", g_alerts.alerts[i]);
            }
        } else {
            Serial.println("[API] Sin alertas activas");
        }
    }

    g_status.last_update = millis();

    // Actualizar UI
    updateDashboardWeather();

    Serial.println("[API] Datos actualizados\n");
}

// ============================================================================
// Actualizar almanaque
// ============================================================================

void updateAlmanac() {
    if (ecowittApi.fetchAlmanac(g_almanac)) {
        Serial.printf("[API] Sol: %s - %s, Luna: %s (%d%%)\n",
                      g_almanac.sunrise,
                      g_almanac.sunset,
                      g_almanac.moon_phase,
                      g_almanac.moon_illumination);
    }
}

// ============================================================================
// Mostrar estado del sistema (debug)
// ============================================================================

void printSystemStatus() {
    Serial.println("\n=== Estado del Sistema ===");
    Serial.printf("WiFi: %s (RSSI: %d dBm)\n",
                  g_status.wifi_connected ? "Conectado" : "Desconectado",
                  g_status.wifi_rssi);
    Serial.printf("API: %s\n", g_status.api_ok ? "OK" : "Error");
    Serial.printf("BME280: %s\n", g_status.bme280_ok ? "OK" : "No disponible");
    Serial.printf("Estación remota: %s\n",
                  g_remote_config.enabled ?
                    (g_status.remote_station_ok ? "Enviando OK" : "Error envío") :
                    "Deshabilitada");
    Serial.printf("Uptime: %lu segundos\n", millis() / 1000);
    Serial.printf("Heap libre: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("PSRAM libre: %d bytes\n", ESP.getFreePsram());
    Serial.println("===========================\n");
}
