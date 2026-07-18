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
#include "time_sync.h"

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
ForecastData g_forecast = {0};
AirQualityData g_airquality = {0};
StatsData g_stats = {0};
SystemStatus g_status = {0};
LocalSensorData g_local = {0};
RemoteSensorData g_jardin = {0};
RemoteGatewayData g_remoto = {0};
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
    // La hora se sincroniza desde el servidor (received_at)
    // No se necesita NTP ni configurar timezone
    Serial.println("[TIME] Hora se sincronizara desde el servidor...");

    // Verificar conexión al servidor
    Serial.println("[API] Verificando conexión al servidor...");
    if (ecowittApi.checkConnection()) {
        Serial.println("[API] Servidor OK");
        g_status.api_ok = true;
    } else {
        Serial.println("[API] No se pudo conectar al servidor");
        g_status.api_ok = false;
    }

    // Primera carga de datos (también sincroniza la hora)
    // Nota: updateWeatherData() ya obtiene almanac via /api/display
    updateWeatherData();

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

    // Aplicar offsets BME280 desde preferencias
    setBME280PressureOffset(userPrefs.bme280_pressure_offset);
    setBME280TemperatureOffset(userPrefs.bme280_temperature_offset);
    Serial.printf("[BME280] Offset presion: %d hPa, temperatura: %+d C\n",
                  userPrefs.bme280_pressure_offset, userPrefs.bme280_temperature_offset);

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
#if REMOTE_STATION_ENABLED
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

    // Register touch input AFTER LVGL is initialized
    registerTouchInput();

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

bool tryConnectWiFi(const char* ssid, const char* pass, int networkNum) {
    if (strlen(ssid) == 0) return false;

    Serial.printf("[WiFi] Red %d: Conectando a %s", networkNum, ssid);

    WiFi.disconnect();
    WiFi.begin(ssid, pass);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" OK!");
        Serial.printf("[WiFi] IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("[WiFi] RSSI: %d dBm\n", WiFi.RSSI());
        return true;
    }

    Serial.println(" FALLO");
    return false;
}

void connectWiFi() {
    WiFi.mode(WIFI_STA);

    UserPreferences* prefs = getPreferences();

    // Intentar las 3 redes configuradas en orden
    if (prefs->configured) {
        for (int i = 1; i <= 3; i++) {
            if (hasWifiNetwork(i)) {
                if (tryConnectWiFi(getWifiSSID(i), getWifiPassword(i), i)) {
                    g_status.wifi_connected = true;
                    g_status.wifi_rssi = WiFi.RSSI();
                    return;
                }
            }
        }
    }

    // Fallback: usar defines de my_config.h
    Serial.println("[WiFi] Intentando red de respaldo (my_config.h)...");
    if (tryConnectWiFi(WIFI_SSID, WIFI_PASSWORD, 0)) {
        g_status.wifi_connected = true;
        g_status.wifi_rssi = WiFi.RSSI();
        return;
    }

    Serial.println("[WiFi] No se pudo conectar a ninguna red!");
    g_status.wifi_connected = false;
}

// ============================================================================
// Actualizar datos meteorológicos del servidor
// ============================================================================

// ============================================================================
// Simulador de datos (para pruebas sin conexión al servidor)
// ============================================================================

void simulateWeatherData() {
    Serial.println("[SIM] Generando datos simulados...");

    // Valores base con variación basada en tiempo
    float base_temp = 22.0 + sin(millis() / 60000.0) * 5.0;  // Oscila ±5°C
    float base_humidity = 55.0 + cos(millis() / 90000.0) * 20.0;  // Oscila ±20%

    // === Principal (WS69) ===
    g_weather.temp_outdoor = base_temp + random(-20, 20) / 10.0;
    g_weather.humidity_outdoor = constrain(base_humidity + random(-5, 5), 20, 100);
    g_weather.pressure_rel = 1013.0 + random(-50, 50) / 10.0;
    g_weather.wind_speed = random(0, 250) / 10.0;
    g_weather.wind_gust = g_weather.wind_speed + random(0, 100) / 10.0;
    g_weather.wind_dir = random(0, 360);
    const char* cardinals[] = {"N","NNE","NE","ENE","E","ESE","SE","SSE",
                               "S","SSO","SO","OSO","O","ONO","NO","NNO"};
    int idx = ((int)(g_weather.wind_dir + 11) / 22) % 16;
    strlcpy(g_weather.wind_dir_cardinal, cardinals[idx], sizeof(g_weather.wind_dir_cardinal));
    g_weather.rain_rate = random(0, 30) / 10.0;
    g_weather.rain_day = random(0, 100) / 10.0;
    g_weather.uv = random(0, 110) / 10.0;
    g_weather.solar_radiation = random(0, 1000);
    g_weather.feels_like = g_weather.temp_outdoor - (g_weather.wind_speed / 10.0);
    g_weather.dewpoint = g_weather.temp_outdoor - ((100 - g_weather.humidity_outdoor) / 5.0);
    g_weather.temp_max = g_weather.temp_outdoor + random(20, 50) / 10.0;
    g_weather.temp_min = g_weather.temp_outdoor - random(20, 50) / 10.0;
    g_weather.valid = true;

    Serial.printf("[SIM] Principal: %.1f°C, %.0f%%, %.1f hPa, Viento: %.1f km/h %s\n",
                  g_weather.temp_outdoor, g_weather.humidity_outdoor,
                  g_weather.pressure_rel, g_weather.wind_speed, g_weather.wind_dir_cardinal);

    // === Local (WS2910 - consola interior) ===
    g_weather.temp_indoor = base_temp + 2.0 + random(-10, 10) / 10.0;
    g_weather.humidity_indoor = constrain(base_humidity - 10 + random(-5, 5), 20, 80);

    Serial.printf("[SIM] Local (WS2910): %.1f°C, %.0f%%\n",
                  g_weather.temp_indoor, g_weather.humidity_indoor);

    // === Jardín (WN31 ch1) ===
    g_jardin.temperature = base_temp - 1.0 + random(-15, 15) / 10.0;
    g_jardin.humidity = constrain(base_humidity + 5 + random(-5, 5), 30, 95);
    g_jardin.battery = 100;  // OK
    g_jardin.temp_max = g_jardin.temperature + 2.0;
    g_jardin.temp_min = g_jardin.temperature - 2.0;
    g_jardin.valid = true;

    Serial.printf("[SIM] Jardin (WN31): %.1f°C, %.0f%%, Bat: %d%%\n",
                  g_jardin.temperature, g_jardin.humidity, g_jardin.battery);

    // === Remota (GW1100) ===
    g_remoto.temperature = base_temp - 3.0 + random(-20, 20) / 10.0;
    g_remoto.humidity = constrain(base_humidity + 10 + random(-5, 5), 40, 100);
    g_remoto.pressure = g_weather.pressure_rel - 5.0 + random(-10, 10) / 10.0;
    g_remoto.temp_max = g_remoto.temperature + 3.0;
    g_remoto.temp_min = g_remoto.temperature - 3.0;
    g_remoto.valid = true;

    Serial.printf("[SIM] Remota (GW1100): %.1f°C, %.0f%%, %.0f hPa\n",
                  g_remoto.temperature, g_remoto.humidity, g_remoto.pressure);

    // === Portable (BME280 simulado) ===
    g_local.temperature = base_temp + 3.0 + random(-10, 10) / 10.0;
    g_local.humidity = constrain(base_humidity - 15 + random(-5, 5), 25, 70);
    g_local.pressure = g_weather.pressure_rel;
    g_local.temp_max = g_local.temperature + 1.0;
    g_local.temp_min = g_local.temperature - 1.0;
    g_local.battery = -1;
    g_local.valid = true;

    Serial.printf("[SIM] Portable: %.1f°C, %.0f%%, %.0f hPa\n",
                  g_local.temperature, g_local.humidity, g_local.pressure);

    // === Comparación vs ayer ===
    g_compare.temp_diff = random(-30, 30) / 10.0;
    g_compare.humidity_diff = random(-15, 15);
    g_compare.pressure_diff = random(-20, 20) / 10.0;
    g_compare.valid = true;

    // === Alertas ===
    g_alerts.has_alerts = (random(0, 10) > 7);  // 30% probabilidad
    if (g_alerts.has_alerts) {
        g_alerts.alert_count = 1;
        strlcpy(g_alerts.alerts[0], "Lluvia ligera esperada", sizeof(g_alerts.alerts[0]));
    } else {
        g_alerts.alert_count = 0;
    }

    g_status.api_ok = true;  // Simulador siempre OK
    g_status.last_update = millis();

    // Actualizar UI
    updateDashboardWeather();

    Serial.println("[SIM] Datos simulados generados\n");
}

void updateWeatherData() {
    // === MODO SIMULADOR ===
    // Descomentar la siguiente línea para usar datos simulados
    // #define USE_SIMULATOR

    #ifdef USE_SIMULATOR
    simulateWeatherData();
    return;
    #endif

    Serial.println("[API] Actualizando datos (endpoint optimizado)...");

    // ========================================================================
    // Endpoint optimizado: UNA sola llamada obtiene todo
    // ========================================================================
    int serverTimezone = -6;  // Default Mexico
    if (ecowittApi.fetchAll(g_weather, g_compare, g_almanac, g_jardin, g_remoto,
                            &serverTimezone, &g_forecast, &g_airquality, &g_stats)) {
        // Sincronizar hora del sistema con el servidor (usando timezone del server)
        if (strlen(g_weather.timestamp) > 0) {
            syncTimeFromServer(g_weather.timestamp, serverTimezone);
        }

        Serial.printf("[API] Principal: %.1f°C, %.0f%%, %.1f hPa\n",
                      g_weather.temp_outdoor,
                      g_weather.humidity_outdoor,
                      g_weather.pressure_rel);
        Serial.printf("[API] Max/Min: %.1f/%.1f°C\n",
                      g_weather.temp_max,
                      g_weather.temp_min);
        Serial.printf("[API] vs ayer: %+.1f°C\n", g_compare.temp_diff);
        Serial.printf("[API] Sol: %s - %s\n", g_almanac.sunrise, g_almanac.sunset);

        if (g_jardin.valid) {
            Serial.printf("[API] Jardin: %.1f°C, %.0f%%\n",
                          g_jardin.temperature, g_jardin.humidity);
        }
        if (g_remoto.valid) {
            Serial.printf("[API] Remoto: %.1f°C, %.0f%%\n",
                          g_remoto.temperature, g_remoto.humidity);
        }
        if (g_forecast.valid && g_forecast.available) {
            Serial.printf("[API] Pronostico: %s\n", g_forecast.forecast);
        }
        if (g_airquality.valid) {
            Serial.printf("[API] AQI: %d (%s)\n", g_airquality.aqi, g_airquality.station);
        }
        if (g_stats.valid && strlen(g_stats.temp_max_time) > 0) {
            Serial.printf("[API] Max %.1f°C (%s), Min %.1f°C (%s)\n",
                          g_stats.temp_max, g_stats.temp_max_time,
                          g_stats.temp_min, g_stats.temp_min_time);
        }

        g_status.api_ok = true;
    } else {
        Serial.println("[API] ERROR en fetchAll");
        g_status.api_ok = false;
        return;
    }

    // Alertas (aún no incluidas en /api/display, se obtienen aparte)
    if (ecowittApi.fetchAlerts(g_alerts)) {
        if (g_alerts.has_alerts) {
            Serial.printf("[API] %d ALERTAS ACTIVAS\n", g_alerts.alert_count);
        }
    }

    // ========================================================================
    // Portable (BME280 local) - simulado si no hay sensor
    // ========================================================================
#ifndef BME280_ENABLED
    // Simular datos del Portable basados en exterior con offset
    g_local.temperature = g_weather.temp_outdoor + 2.5f;  // Interior más cálido
    g_local.humidity = g_weather.humidity_outdoor - 10.0f;  // Menos humedad
    g_local.pressure = g_weather.pressure_rel;
    g_local.temp_max = g_local.temperature + 1.0f;
    g_local.temp_min = g_local.temperature - 1.0f;
    g_local.battery = -1;  // No aplica
    g_local.valid = true;
    g_local.last_read = millis();
    Serial.printf("[SIM] Portable: %.1f°C, %.0f%%, %.0f hPa (simulado)\n",
                  g_local.temperature, g_local.humidity, g_local.pressure);
#endif

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
