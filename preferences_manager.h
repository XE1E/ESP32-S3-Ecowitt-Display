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
    // WiFi
    char wifi_ssid[33];
    char wifi_pass[65];

    // Servidor
    char server_url[128];

    // Unidades
    bool use_celsius;       // true = °C, false = °F
    bool use_24h;           // true = 24h, false = 12h
    bool use_hpa;           // true = hPa, false = mmHg
    bool use_kmh;           // true = km/h, false = mph

    // Display
    uint8_t brightness;     // 0-255
    bool auto_brightness;   // Ajuste automático
    uint16_t sleep_timeout; // Segundos antes de dim (0 = desactivado)

    // Estación remota
    bool remote_enabled;
    char remote_passkey[33];
    char remote_label[32];
    uint16_t remote_interval;

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

    userPrefs.remote_enabled = false;
    userPrefs.remote_interval = 60;

    userPrefs.configured = false;
    userPrefs.boot_count = 0;
}

// ============================================================================
// Cargar preferencias de NVS
// ============================================================================

bool loadPreferences() {
    nvs.begin("ecowitt", true);  // Read-only

    // WiFi
    String ssid = nvs.getString("wifi_ssid", "");
    String pass = nvs.getString("wifi_pass", "");
    strncpy(userPrefs.wifi_ssid, ssid.c_str(), sizeof(userPrefs.wifi_ssid) - 1);
    strncpy(userPrefs.wifi_pass, pass.c_str(), sizeof(userPrefs.wifi_pass) - 1);

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

    // Estación remota
    userPrefs.remote_enabled = nvs.getBool("remote_en", false);
    String rkey = nvs.getString("remote_key", "");
    String rlbl = nvs.getString("remote_lbl", "Display");
    strncpy(userPrefs.remote_passkey, rkey.c_str(), sizeof(userPrefs.remote_passkey) - 1);
    strncpy(userPrefs.remote_label, rlbl.c_str(), sizeof(userPrefs.remote_label) - 1);
    userPrefs.remote_interval = nvs.getUShort("remote_int", 60);

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

    // WiFi
    nvs.putString("wifi_ssid", userPrefs.wifi_ssid);
    nvs.putString("wifi_pass", userPrefs.wifi_pass);

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

    // Estación remota
    nvs.putBool("remote_en", userPrefs.remote_enabled);
    nvs.putString("remote_key", userPrefs.remote_passkey);
    nvs.putString("remote_lbl", userPrefs.remote_label);
    nvs.putUShort("remote_int", userPrefs.remote_interval);

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

const char* getWifiSSID() {
    return userPrefs.wifi_ssid;
}

const char* getWifiPassword() {
    return userPrefs.wifi_pass;
}

const char* getServerURL() {
    return userPrefs.server_url;
}

// ============================================================================
// Setters con guardado automático
// ============================================================================

void setWifiCredentials(const char* ssid, const char* pass) {
    strncpy(userPrefs.wifi_ssid, ssid, sizeof(userPrefs.wifi_ssid) - 1);
    strncpy(userPrefs.wifi_pass, pass, sizeof(userPrefs.wifi_pass) - 1);
    savePreferences();
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
