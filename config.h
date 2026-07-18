/**
 * config.h - Estructuras de datos y configuración global
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// Pines del hardware
// ============================================================================

// Display RGB (Waveshare ESP32-S3-Touch-LCD-7B)
// Pines según rgb_lcd_port.h original de Waveshare
#define LCD_DE    5
#define LCD_VSYNC 3
#define LCD_HSYNC 46
#define LCD_PCLK  7
// Red (was Blue in original - B3-B7)
#define LCD_R0    14
#define LCD_R1    38
#define LCD_R2    18
#define LCD_R3    17
#define LCD_R4    10
// Green (G2-G7 -> G0-G5 for Arduino_GFX)
#define LCD_G0    39
#define LCD_G1    0
#define LCD_G2    45
#define LCD_G3    48
#define LCD_G4    47
#define LCD_G5    21
// Blue (was Red in original - R3-R7)
#define LCD_B0    1
#define LCD_B1    2
#define LCD_B2    42
#define LCD_B3    41
#define LCD_B4    40
#define LCD_BL    -1  // Backlight manejado por CH422G

// I2C Bus (shared by CH422G, GT911, BME280)
// IMPORTANT: Pins 8/9, NOT 19/20!
#define I2C_SDA       8
#define I2C_SCL       9

// Touch (GT911) - uses shared I2C
#define TOUCH_SDA     I2C_SDA
#define TOUCH_SCL     I2C_SCL
#define TOUCH_INT     4
// TOUCH_RST is controlled via CH422G IO1, not GPIO

// BME280 (shares I2C bus)
#define BME280_SDA    I2C_SDA
#define BME280_SCL    I2C_SCL

// ============================================================================
// Resolución del display
// ============================================================================

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 600

// ============================================================================
// Estructuras de datos
// ============================================================================

// Datos del BME280 local (este display)
struct LocalSensorData {
    float temperature;
    float humidity;
    float pressure;
    float temp_max;
    float temp_min;
    int battery;              // 0-100 o -1 si no aplica
    bool valid;
    unsigned long last_read;
};

// Datos de sensor remoto generico (HN31, WN31, etc)
struct RemoteSensorData {
    char id[16];              // Identificador (ej: "HN31_jardin")
    char label[24];           // Nombre visible (ej: "Jardin")
    float temperature;
    float humidity;
    float temp_max;
    float temp_min;
    int battery;              // 0-100%
    bool valid;
    unsigned long last_update;
};

// Datos de gateway remoto (GW110, GW1100, etc)
struct RemoteGatewayData {
    char id[16];              // Identificador
    char label[24];           // Nombre visible
    float temperature;
    float humidity;
    float pressure;
    float temp_max;
    float temp_min;
    bool valid;
    unsigned long last_update;
};

// Datos meteorológicos actuales (del servidor)
struct WeatherData {
    // Exterior (de la estación Ecowitt principal)
    float temp_outdoor;
    float humidity_outdoor;
    float pressure_rel;
    float wind_speed;
    float wind_gust;
    float wind_dir;           // Degrees (0-360)
    char wind_dir_cardinal[4];
    float rain_rate;
    float rain_day;
    float rain_week;
    float rain_month;
    float rain_year;
    float uv;
    float solar_radiation;
    float feels_like;
    float dewpoint;

    // Interior (de la consola Ecowitt)
    float temp_indoor;
    float humidity_indoor;

    // Batería sensores (true=OK, false=baja)
    bool battery_wh65;        // WS69 sensor exterior

    // Estadísticas del día
    float temp_max;
    float temp_min;
    float wind_max;
    float rain_total;

    // Condicion derivada
    char condition[16];       // "clear", "cloudy", "rain", "storm", "fog", "wind"
    bool is_day;              // true si es de dia (para iconos)

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
    char sunrise[10];    // "06:06 a.m."
    char sunset[10];     // "07:18 p.m."
    char moon_phase[32];
    int moon_illumination;
    int daylight_hours;
    int daylight_minutes;
    bool valid;
};

// Pronóstico barométrico
struct ForecastData {
    char forecast[96];       // Texto del pronóstico
    char trend_code[16];     // "steady", "rising", "falling", etc.
    char trend_label[24];    // "estable", "subiendo", etc.
    char trend_arrow[8];     // "→", "↑", "↓", etc.
    float pressure;          // Presión actual
    float delta_3h;          // Cambio en 3 horas
    bool available;
    bool valid;
};

// Calidad del aire
struct AirQualityData {
    int aqi;                 // Índice AQI (0-500)
    float pm25;              // PM2.5 en μg/m³
    float pm10;              // PM10 en μg/m³
    char station[32];        // Estación de monitoreo
    char dominant[16];       // Contaminante dominante
    bool valid;
};

// Estadísticas con hora de extremos
struct StatsData {
    float temp_max;
    float temp_min;
    char temp_max_time[6];   // "14:30"
    char temp_min_time[6];   // "06:15"
    float wind_max;
    char wind_max_time[6];
    bool valid;
};

// Punto de datos históricos
struct HistoryPoint {
    time_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    float rain;
};

// Datos históricos (máx 288 puntos = 24h x 12 por hora = cada 5 min)
#define HISTORY_MAX_POINTS 144  // 24h x 6 por hora = cada 10 min
struct HistoryData {
    HistoryPoint points[HISTORY_MAX_POINTS];
    int count;
    float temp_min;
    float temp_max;
    float temp_avg;
    float hum_min;
    float hum_max;
    float pres_min;
    float pres_max;
    bool valid;
};

// Estado del sistema
struct SystemStatus {
    bool wifi_connected;
    int wifi_rssi;
    bool api_ok;
    bool bme280_ok;
    bool remote_station_ok;      // Último envío como estación remota exitoso
    unsigned long last_update;
    unsigned long last_bme_read;
    unsigned long last_remote_send;
    unsigned long uptime;
};

// Configuración de estación remota (este display como sensor)
struct RemoteStationConfig {
    bool enabled;                // Enviar datos BME280 al servidor
    char passkey[33];            // Identificador único (ej: "display_sala")
    char label[32];              // Nombre visible (ej: "Sala")
    int send_interval;           // Intervalo de envío en segundos (min 60)
};

// ============================================================================
// Variables globales
// ============================================================================

extern WeatherData g_weather;
extern CompareData g_compare;
extern AlertData g_alerts;
extern AlmanacData g_almanac;
extern ForecastData g_forecast;
extern AirQualityData g_airquality;
extern StatsData g_stats;
extern SystemStatus g_status;
extern LocalSensorData g_local;
extern RemoteSensorData g_jardin;      // HN31 en jardin
extern RemoteGatewayData g_remoto;     // GW110 remoto
extern RemoteStationConfig g_remote_config;

#endif // CONFIG_H
