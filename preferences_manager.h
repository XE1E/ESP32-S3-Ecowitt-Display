/**
 * preferences_manager.h - Gestión de preferencias persistentes
 *
 * Almacena en NVS:
 * - Configuración WiFi
 * - URL del servidor
 * - Unidades (°C/°F, 12h/24h)
 * - Brillo del display
 * - Preferencias de UI
 */

#ifndef PREFERENCES_MANAGER_H
#define PREFERENCES_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

// ============================================================================
// Estructura de preferencias
// ============================================================================

struct UserPreferences {
    // WiFi (hasta 3 redes con fallback automatico)
    char wifi_ssid1[33];
    char wifi_pass1[65];
    char wifi_ssid2[33];
    char wifi_pass2[65];
    char wifi_ssid3[33];
    char wifi_pass3[65];

    // Servidor
    char server_url[128];

    // Unidades
    bool use_celsius;       // true = °C, false = °F
    bool use_24h;           // true = 24h, false = 12h
    bool use_hpa;           // true = hPa, false = mmHg
    bool use_kmh;           // true = km/h, false = mph

    // Display
    uint8_t brightness;     // 0-255
    bool auto_brightness;   // Ajuste automatico
    uint16_t sleep_timeout; // Segundos antes de dim (0 = desactivado)
    bool dark_mode;         // Tema oscuro

    // Estación remota
    bool remote_enabled;
    char remote_passkey[33];
    char remote_label[32];
    uint16_t remote_interval;

    // BME280
    int8_t bme280_pressure_offset;     // Offset de presion en hPa (-15 a +15)
    int8_t bme280_temperature_offset;  // Offset de temperatura en grados (-15 a +15)

    // Estado
    bool configured;
    uint32_t boot_count;
};

static UserPreferences userPrefs;
static Preferences nvs;

// ============================================================================
// Valores por defecto
// ============================================================================

void setDefaultPreferences() {
    memset(&userPrefs, 0, sizeof(userPrefs));

    strcpy(userPrefs.server_url, "https://clima.xe1e.net");

    userPrefs.use_celsius = true;
    userPrefs.use_24h = true;
    userPrefs.use_hpa = true;
    userPrefs.use_kmh = true;

    userPrefs.brightness = 200;
    userPrefs.auto_brightness = false;
    userPrefs.sleep_timeout = 0;
    userPrefs.dark_mode = false;

    userPrefs.remote_enabled = false;
    userPrefs.remote_interval = 60;

    userPrefs.bme280_pressure_offset = 0;
    userPrefs.bme280_temperature_offset = 0;

    userPrefs.configured = false;
    userPrefs.boot_count = 0;
}

// ============================================================================
// Cargar preferencias de NVS
// ============================================================================

bool loadPreferences() {
    nvs.begin("ecowitt", true);  // Read-only

    // WiFi (3 redes) - intentar nuevas claves primero
    String ssid1 = nvs.getString("wifi_ssid1", "");
    String pass1 = nvs.getString("wifi_pass1", "");

    // Migración: si no hay wifi_ssid1, intentar claves antiguas
    if (ssid1.length() == 0) {
        ssid1 = nvs.getString("wifi_ssid", "");
        pass1 = nvs.getString("wifi_pass", "");
        if (ssid1.length() > 0) {
            Serial.println("[PREFS] Migrando WiFi de claves antiguas...");
        }
    }

    strncpy(userPrefs.wifi_ssid1, ssid1.c_str(), sizeof(userPrefs.wifi_ssid1) - 1);
    strncpy(userPrefs.wifi_pass1, pass1.c_str(), sizeof(userPrefs.wifi_pass1) - 1);

    String ssid2 = nvs.getString("wifi_ssid2", "");
    String pass2 = nvs.getString("wifi_pass2", "");
    strncpy(userPrefs.wifi_ssid2, ssid2.c_str(), sizeof(userPrefs.wifi_ssid2) - 1);
    strncpy(userPrefs.wifi_pass2, pass2.c_str(), sizeof(userPrefs.wifi_pass2) - 1);

    String ssid3 = nvs.getString("wifi_ssid3", "");
    String pass3 = nvs.getString("wifi_pass3", "");
    strncpy(userPrefs.wifi_ssid3, ssid3.c_str(), sizeof(userPrefs.wifi_ssid3) - 1);
    strncpy(userPrefs.wifi_pass3, pass3.c_str(), sizeof(userPrefs.wifi_pass3) - 1);

    // Servidor
    String url = nvs.getString("server_url", "https://clima.xe1e.net");
    strncpy(userPrefs.server_url, url.c_str(), sizeof(userPrefs.server_url) - 1);

    // Unidades
    userPrefs.use_celsius = nvs.getBool("use_celsius", true);
    userPrefs.use_24h = nvs.getBool("use_24h", true);
    userPrefs.use_hpa = nvs.getBool("use_hpa", true);
    userPrefs.use_kmh = nvs.getBool("use_kmh", true);

    // Display
    userPrefs.brightness = nvs.getUChar("brightness", 200);
    userPrefs.auto_brightness = nvs.getBool("auto_bright", false);
    userPrefs.sleep_timeout = nvs.getUShort("sleep_time", 0);
    userPrefs.dark_mode = nvs.getBool("dark_mode", false);

    // Estación remota
    userPrefs.remote_enabled = nvs.getBool("remote_en", false);
    String rkey = nvs.getString("remote_key", "");
    String rlbl = nvs.getString("remote_lbl", "Display");
    strncpy(userPrefs.remote_passkey, rkey.c_str(), sizeof(userPrefs.remote_passkey) - 1);
    strncpy(userPrefs.remote_label, rlbl.c_str(), sizeof(userPrefs.remote_label) - 1);
    userPrefs.remote_interval = nvs.getUShort("remote_int", 60);

    // BME280
    userPrefs.bme280_pressure_offset = nvs.getChar("bme_pres_off", 0);
    userPrefs.bme280_temperature_offset = nvs.getChar("bme_temp_off", 0);

    // Estado
    userPrefs.configured = nvs.getBool("configured", false);
    userPrefs.boot_count = nvs.getUInt("boot_count", 0);

    nvs.end();

    Serial.printf("[PREFS] Loaded (configured=%d, boots=%d)\n",
                  userPrefs.configured, userPrefs.boot_count);

    return userPrefs.configured;
}

// ============================================================================
// Guardar preferencias en NVS
// ============================================================================

void savePreferences() {
    nvs.begin("ecowitt", false);  // Read-write

    // WiFi (3 redes)
    nvs.putString("wifi_ssid1", userPrefs.wifi_ssid1);
    nvs.putString("wifi_pass1", userPrefs.wifi_pass1);
    nvs.putString("wifi_ssid2", userPrefs.wifi_ssid2);
    nvs.putString("wifi_pass2", userPrefs.wifi_pass2);
    nvs.putString("wifi_ssid3", userPrefs.wifi_ssid3);
    nvs.putString("wifi_pass3", userPrefs.wifi_pass3);

    // Servidor
    nvs.putString("server_url", userPrefs.server_url);

    // Unidades
    nvs.putBool("use_celsius", userPrefs.use_celsius);
    nvs.putBool("use_24h", userPrefs.use_24h);
    nvs.putBool("use_hpa", userPrefs.use_hpa);
    nvs.putBool("use_kmh", userPrefs.use_kmh);

    // Display
    nvs.putUChar("brightness", userPrefs.brightness);
    nvs.putBool("auto_bright", userPrefs.auto_brightness);
    nvs.putUShort("sleep_time", userPrefs.sleep_timeout);
    nvs.putBool("dark_mode", userPrefs.dark_mode);

    // Estación remota
    nvs.putBool("remote_en", userPrefs.remote_enabled);
    nvs.putString("remote_key", userPrefs.remote_passkey);
    nvs.putString("remote_lbl", userPrefs.remote_label);
    nvs.putUShort("remote_int", userPrefs.remote_interval);

    // BME280
    nvs.putChar("bme_pres_off", userPrefs.bme280_pressure_offset);
    nvs.putChar("bme_temp_off", userPrefs.bme280_temperature_offset);

    // Estado
    nvs.putBool("configured", userPrefs.configured);

    nvs.end();

    Serial.println("[PREFS] Saved");
}

// ============================================================================
// Incrementar contador de arranques
// ============================================================================

void incrementBootCount() {
    nvs.begin("ecowitt", false);
    uint32_t count = nvs.getUInt("boot_count", 0) + 1;
    nvs.putUInt("boot_count", count);
    userPrefs.boot_count = count;
    nvs.end();

    Serial.printf("[PREFS] Boot count: %d\n", count);
}

// ============================================================================
// Reset a valores de fábrica
// ============================================================================

void resetPreferences() {
    nvs.begin("ecowitt", false);
    nvs.clear();
    nvs.end();

    setDefaultPreferences();
    Serial.println("[PREFS] Reset to defaults");
}

// ============================================================================
// Getters para uso externo
// ============================================================================

UserPreferences* getPreferences() {
    return &userPrefs;
}

bool isConfigured() {
    return userPrefs.configured;
}

const char* getWifiSSID(int network = 1) {
    switch(network) {
        case 1: return userPrefs.wifi_ssid1;
        case 2: return userPrefs.wifi_ssid2;
        case 3: return userPrefs.wifi_ssid3;
        default: return userPrefs.wifi_ssid1;
    }
}

const char* getWifiPassword(int network = 1) {
    switch(network) {
        case 1: return userPrefs.wifi_pass1;
        case 2: return userPrefs.wifi_pass2;
        case 3: return userPrefs.wifi_pass3;
        default: return userPrefs.wifi_pass1;
    }
}

bool hasWifiNetwork(int network) {
    return strlen(getWifiSSID(network)) > 0;
}

const char* getServerURL() {
    return userPrefs.server_url;
}

// ============================================================================
// Setters con guardado automático
// ============================================================================

void setWifiCredentials(int network, const char* ssid, const char* pass) {
    switch(network) {
        case 1:
            strncpy(userPrefs.wifi_ssid1, ssid, sizeof(userPrefs.wifi_ssid1) - 1);
            strncpy(userPrefs.wifi_pass1, pass, sizeof(userPrefs.wifi_pass1) - 1);
            break;
        case 2:
            strncpy(userPrefs.wifi_ssid2, ssid, sizeof(userPrefs.wifi_ssid2) - 1);
            strncpy(userPrefs.wifi_pass2, pass, sizeof(userPrefs.wifi_pass2) - 1);
            break;
        case 3:
            strncpy(userPrefs.wifi_ssid3, ssid, sizeof(userPrefs.wifi_ssid3) - 1);
            strncpy(userPrefs.wifi_pass3, pass, sizeof(userPrefs.wifi_pass3) - 1);
            break;
    }
    savePreferences();
}

void clearWifiCredentials(int network) {
    setWifiCredentials(network, "", "");
}

void setServerURL(const char* url) {
    strncpy(userPrefs.server_url, url, sizeof(userPrefs.server_url) - 1);
    savePreferences();
}

void setUseCelsius(bool celsius) {
    userPrefs.use_celsius = celsius;
    savePreferences();
}

void setUse24Hour(bool use24h) {
    userPrefs.use_24h = use24h;
    savePreferences();
}

void setBrightness(uint8_t brightness) {
    userPrefs.brightness = brightness;
    savePreferences();
    // Aplicar inmediatamente
    // analogWrite(LCD_BACKLIGHT, brightness);
}

void setConfigured(bool configured) {
    userPrefs.configured = configured;
    savePreferences();
}

// ============================================================================
// Conversiones de unidades
// ============================================================================

float convertTemperature(float celsius) {
    if (userPrefs.use_celsius) {
        return celsius;
    }
    return celsius * 9.0 / 5.0 + 32.0;  // Fahrenheit
}

const char* getTemperatureUnit() {
    return userPrefs.use_celsius ? "°C" : "°F";
}

float convertPressure(float hpa) {
    if (userPrefs.use_hpa) {
        return hpa;
    }
    return hpa * 0.750062;  // mmHg
}

const char* getPressureUnit() {
    return userPrefs.use_hpa ? "hPa" : "mmHg";
}

float convertSpeed(float kmh) {
    if (userPrefs.use_kmh) {
        return kmh;
    }
    return kmh * 0.621371;  // mph
}

const char* getSpeedUnit() {
    return userPrefs.use_kmh ? "km/h" : "mph";
}

String formatTime(int hour, int minute) {
    char buf[10];
    if (userPrefs.use_24h) {
        snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);
    } else {
        int h12 = hour % 12;
        if (h12 == 0) h12 = 12;
        const char* ampm = hour < 12 ? "AM" : "PM";
        snprintf(buf, sizeof(buf), "%d:%02d %s", h12, minute, ampm);
    }
    return String(buf);
}

#endif // PREFERENCES_MANAGER_H
