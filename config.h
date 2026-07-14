/**
 * config.h - Estructuras de datos y configuración global
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// Pines del hardware
// ============================================================================

// Touch (GT911)
#define TOUCH_SDA     19
#define TOUCH_SCL     20
#define TOUCH_RST     38
#define TOUCH_INT     18

// BME280 (comparte I2C con touch)
#define BME280_SDA    19
#define BME280_SCL    20

// Display
#define LCD_BACKLIGHT 2

// ============================================================================
// Resolución del display
// ============================================================================

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 600

// ============================================================================
// Estructuras de datos
// ============================================================================

// Datos meteorológicos actuales
struct WeatherData {
    // Exterior (de la estación Ecowitt)
    float temp_outdoor;
    float humidity_outdoor;
    float pressure_rel;
    float wind_speed;
    float wind_gust;
    char wind_dir_cardinal[4];
    float rain_rate;
    float rain_day;
    float uv;
    float solar_radiation;
    float feels_like;
    float dewpoint;

    // Interior (de la consola Ecowitt)
    float temp_indoor;
    float humidity_indoor;

    // Estadísticas del día
    float temp_max;
    float temp_min;
    float wind_max;
    float rain_total;

    // Metadata
    char timestamp[32];
    bool valid;
};

// Datos de comparación (vs ayer)
struct CompareData {
    float temp_diff;
    float humidity_diff;
    float pressure_diff;
    bool valid;
};

// Alertas activas
struct AlertData {
    bool has_alerts;
    int alert_count;
    char alerts[5][64];  // Hasta 5 alertas, 64 chars cada una
};

// Datos del almanaque
struct AlmanacData {
    char sunrise[6];     // "07:15"
    char sunset[6];      // "19:42"
    char moon_phase[32];
    int moon_illumination;
    bool valid;
};

// Estado del sistema
struct SystemStatus {
    bool wifi_connected;
    int wifi_rssi;
    bool api_ok;
    unsigned long last_update;
    unsigned long uptime;
};

// ============================================================================
// Variables globales
// ============================================================================

extern WeatherData g_weather;
extern CompareData g_compare;
extern AlertData g_alerts;
extern AlmanacData g_almanac;
extern SystemStatus g_status;

#endif // CONFIG_H
