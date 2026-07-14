/**
 * ESP32-S3-Ecowitt-Display
 *
 * Display táctil de 7" para estación meteorológica Ecowitt.
 * Muestra datos en tiempo real desde tu propio servidor.
 *
 * Hardware: Waveshare ESP32-S3-Touch-LCD-7B (1024x600)
 * Framework: LVGL 8.3
 *
 * https://github.com/XE1E/ESP32-S3-Ecowitt-Display
 */

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

#include "config.h"
#include "my_config.h"
#include "ecowitt_api.h"

// TODO: Incluir cuando estén implementados
// #include "lvgl_port.h"
// #include "ui.h"

// ============================================================================
// Variables globales
// ============================================================================

WeatherData g_weather = {0};
CompareData g_compare = {0};
AlertData g_alerts = {0};
AlmanacData g_almanac = {0};
SystemStatus g_status = {0};

EcowittAPI ecowittApi;
Preferences preferences;

unsigned long lastUpdateTime = 0;

// ============================================================================
// Setup
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n\n=================================");
    Serial.println("ESP32-S3 Ecowitt Display");
    Serial.println("=================================\n");

    // Verificar PSRAM
    if (psramFound()) {
        Serial.printf("PSRAM: %.2f MB disponible\n",
                      ESP.getFreePsram() / 1024.0 / 1024.0);
    } else {
        Serial.println("ERROR: PSRAM no encontrada!");
        Serial.println("Verifica configuracion: PSRAM = OPI PSRAM");
        while (1) delay(1000);
    }

    // TODO: Inicializar display y touch
    // initDisplay();
    // initTouch();

    // TODO: Inicializar LVGL
    // initLVGL();

    // Conectar WiFi
    connectWiFi();

    // TODO: Crear UI
    // createUI();

    // Primera carga de datos
    updateWeatherData();

    Serial.println("\nSetup completo!\n");
}

// ============================================================================
// Loop principal
// ============================================================================

void loop() {
    // TODO: Procesar LVGL
    // lv_timer_handler();

    // Actualizar datos periódicamente
    if (millis() - lastUpdateTime >= UPDATE_INTERVAL_MS) {
        lastUpdateTime = millis();
        updateWeatherData();
    }

    // Pequeña pausa para no saturar CPU
    delay(5);
}

// ============================================================================
// Funciones auxiliares
// ============================================================================

void connectWiFi() {
    Serial.printf("Conectando a WiFi: %s", WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" OK!");
        Serial.printf("IP: %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
        g_status.wifi_connected = true;
        g_status.wifi_rssi = WiFi.RSSI();
    } else {
        Serial.println(" FALLO!");
        g_status.wifi_connected = false;
        // TODO: Mostrar pantalla de configuración WiFi
    }
}

void updateWeatherData() {
    Serial.println("Actualizando datos...");

    // Datos actuales
    if (ecowittApi.fetchCurrent(g_weather)) {
        Serial.printf("  Temp: %.1f°C, Humedad: %.0f%%\n",
                      g_weather.temp_outdoor,
                      g_weather.humidity_outdoor);
        g_status.api_ok = true;
    } else {
        Serial.println("  ERROR: No se pudo obtener datos actuales");
        g_status.api_ok = false;
    }

    // Estadísticas del día
    if (ecowittApi.fetchDailyStats(g_weather)) {
        Serial.printf("  Max: %.1f°C, Min: %.1f°C\n",
                      g_weather.temp_max,
                      g_weather.temp_min);
    }

    // Comparación vs ayer
    if (ecowittApi.fetchCompare(g_compare)) {
        Serial.printf("  vs ayer: %+.1f°C\n", g_compare.temp_diff);
    }

    // Alertas
    if (ecowittApi.fetchAlerts(g_alerts)) {
        if (g_alerts.has_alerts) {
            Serial.printf("  ALERTAS: %d activas\n", g_alerts.alert_count);
        } else {
            Serial.println("  Sin alertas");
        }
    }

    // Almanaque (menos frecuente)
    static unsigned long lastAlmanacUpdate = 0;
    if (millis() - lastAlmanacUpdate >= 600000) { // Cada 10 min
        lastAlmanacUpdate = millis();
        if (ecowittApi.fetchAlmanac(g_almanac)) {
            Serial.printf("  Sol: %s - %s\n",
                          g_almanac.sunrise,
                          g_almanac.sunset);
        }
    }

    g_status.last_update = millis();
    g_status.uptime = millis() / 1000;

    // TODO: Actualizar UI
    // updateUI();

    Serial.println("Datos actualizados.\n");
}
