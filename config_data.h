/**
 * config_data.h - Configuration Data Structure
 *
 * Persistent storage for all user settings
 */

#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H

#include <Arduino.h>
#include <Preferences.h>
#include "my_config.h"  // Personal credentials (not in git)

// ============================================================================
// Configuration Structure
// ============================================================================

struct WiFiNetwork {
    char ssid[33];
    char password[65];
    bool enabled;
};

struct LocationConfig {
    char city[64];
    char latitude[16];
    char longitude[16];
    char station_id[32];      // WeatherAPI station ID (optional)
    char station_name[64];    // Display name
    char country[32];
};

struct TimezoneConfig {
    char tz_name[32];         // e.g., "America/Mexico_City"
    int gmt_offset;           // Offset in seconds from UTC
    int dst_offset;           // DST offset in seconds (0 or 3600)
    bool auto_dst;            // Automatic DST adjustment
};

struct DisplayConfig {
    uint8_t brightness;       // 0-255
    uint8_t background_id;    // Index of background image
    bool show_seconds;        // Show seconds in clock
    bool use_24h;             // 24-hour format
    bool use_metric;          // true = Celsius/km/h, false = Fahrenheit/mph
    int update_interval;      // Weather update in minutes
    int screen_timeout;       // Screen dim timeout (0 = never)
};

struct APIConfig {
    char weather_api_key[65];
    char thingspeak_key[33];       // For data upload (future)
    char thingspeak_channel[16];   // Channel ID
    bool thingspeak_enabled;
};

struct AlarmConfig {
    uint8_t hour;
    uint8_t minute;
    bool enabled;
    bool days[7];             // Sun=0 to Sat=6
    char label[32];
};

#define MAX_ALARMS 5

struct AppConfig {
    // Version for migration
    uint8_t config_version;

    // WiFi (up to 3 networks)
    WiFiNetwork wifi[3];

    // Location
    LocationConfig location;

    // Timezone
    TimezoneConfig timezone;

    // Display
    DisplayConfig display;

    // API Keys
    APIConfig api;

    // Alarms (future)
    AlarmConfig alarms[MAX_ALARMS];

    // Flags
    bool first_boot;          // True if never configured
    bool setup_complete;      // Setup wizard completed
};

// ============================================================================
// Global Configuration Instance
// ============================================================================

static AppConfig appConfig;
static Preferences configPrefs;

// ============================================================================
// Default Values
// ============================================================================

void setConfigDefaults() {
    appConfig.config_version = 1;

    // WiFi from my_config.h
    strlcpy(appConfig.wifi[0].ssid, MY_WIFI_SSID_1, sizeof(appConfig.wifi[0].ssid));
    strlcpy(appConfig.wifi[0].password, MY_WIFI_PASSWORD_1, sizeof(appConfig.wifi[0].password));
    appConfig.wifi[0].enabled = (strlen(MY_WIFI_SSID_1) > 0);

    strlcpy(appConfig.wifi[1].ssid, MY_WIFI_SSID_2, sizeof(appConfig.wifi[1].ssid));
    strlcpy(appConfig.wifi[1].password, MY_WIFI_PASSWORD_2, sizeof(appConfig.wifi[1].password));
    appConfig.wifi[1].enabled = (strlen(MY_WIFI_SSID_2) > 0);

    strlcpy(appConfig.wifi[2].ssid, MY_WIFI_SSID_3, sizeof(appConfig.wifi[2].ssid));
    strlcpy(appConfig.wifi[2].password, MY_WIFI_PASSWORD_3, sizeof(appConfig.wifi[2].password));
    appConfig.wifi[2].enabled = (strlen(MY_WIFI_SSID_3) > 0);

    // Location from my_config.h
    strlcpy(appConfig.location.city, MY_CITY, sizeof(appConfig.location.city));
    strlcpy(appConfig.location.latitude, MY_LATITUDE, sizeof(appConfig.location.latitude));
    strlcpy(appConfig.location.longitude, MY_LONGITUDE, sizeof(appConfig.location.longitude));
    memset(appConfig.location.station_id, 0, sizeof(appConfig.location.station_id));
    strlcpy(appConfig.location.station_name, MY_CITY, sizeof(appConfig.location.station_name));
    strlcpy(appConfig.location.country, MY_COUNTRY, sizeof(appConfig.location.country));

    // Timezone from my_config.h
    strlcpy(appConfig.timezone.tz_name, MY_TIMEZONE_NAME, sizeof(appConfig.timezone.tz_name));
    appConfig.timezone.gmt_offset = MY_GMT_OFFSET;
    appConfig.timezone.dst_offset = 0;
    appConfig.timezone.auto_dst = false;

    // Display defaults
    appConfig.display.brightness = 80;
    appConfig.display.background_id = 0;
    appConfig.display.show_seconds = false;
    appConfig.display.use_24h = true;
    appConfig.display.use_metric = true;
    appConfig.display.update_interval = 10;
    appConfig.display.screen_timeout = 0;

    // API key from my_config.h
    strlcpy(appConfig.api.weather_api_key, MY_WEATHER_API_KEY, sizeof(appConfig.api.weather_api_key));
    memset(appConfig.api.thingspeak_key, 0, sizeof(appConfig.api.thingspeak_key));
    memset(appConfig.api.thingspeak_channel, 0, sizeof(appConfig.api.thingspeak_channel));
    appConfig.api.thingspeak_enabled = false;

    // Alarms default
    for (int i = 0; i < MAX_ALARMS; i++) {
        appConfig.alarms[i].hour = 7;
        appConfig.alarms[i].minute = 0;
        appConfig.alarms[i].enabled = false;
        memset(appConfig.alarms[i].days, 0, sizeof(appConfig.alarms[i].days));
        memset(appConfig.alarms[i].label, 0, sizeof(appConfig.alarms[i].label));
    }

    // Skip wizard - config comes from my_config.h
    appConfig.first_boot = false;
    appConfig.setup_complete = true;
}

// ============================================================================
// Save/Load Configuration
// ============================================================================

bool saveConfig() {
    configPrefs.begin("estacion", false);

    // Save as binary blob
    size_t written = configPrefs.putBytes("config", &appConfig, sizeof(AppConfig));

    configPrefs.end();

    Serial.printf("Config saved: %d bytes\n", written);
    return written == sizeof(AppConfig);
}

bool loadConfig() {
    configPrefs.begin("estacion", true);

    size_t size = configPrefs.getBytesLength("config");

    if (size == 0) {
        Serial.println("No config found, using defaults");
        configPrefs.end();
        setConfigDefaults();
        return false;
    }

    if (size != sizeof(AppConfig)) {
        Serial.printf("Config size mismatch: %d vs %d, using defaults\n", size, sizeof(AppConfig));
        configPrefs.end();
        setConfigDefaults();
        return false;
    }

    configPrefs.getBytes("config", &appConfig, sizeof(AppConfig));
    configPrefs.end();

    Serial.println("Config loaded successfully");
    return true;
}

// ============================================================================
// Helper Functions
// ============================================================================

bool isWiFiConfigured() {
    return appConfig.wifi[0].enabled && strlen(appConfig.wifi[0].ssid) > 0;
}

bool isAPIKeyConfigured() {
    return strlen(appConfig.api.weather_api_key) > 0;
}

bool isLocationConfigured() {
    return strlen(appConfig.location.latitude) > 0 &&
           strlen(appConfig.location.longitude) > 0;
}

bool isSetupRequired() {
    return !appConfig.setup_complete ||
           !isWiFiConfigured() ||
           !isAPIKeyConfigured();
}

// Get active WiFi credentials (tries in order)
int getActiveWiFi(char *ssid, char *password) {
    for (int i = 0; i < 3; i++) {
        if (appConfig.wifi[i].enabled && strlen(appConfig.wifi[i].ssid) > 0) {
            strcpy(ssid, appConfig.wifi[i].ssid);
            strcpy(password, appConfig.wifi[i].password);
            return i;
        }
    }
    return -1;
}

// Mark setup as complete
void completeSetup() {
    appConfig.first_boot = false;
    appConfig.setup_complete = true;
    saveConfig();
}

// Reset to factory defaults
void factoryReset() {
    configPrefs.begin("estacion", false);
    configPrefs.clear();
    configPrefs.end();
    setConfigDefaults();
    Serial.println("Factory reset complete");
}

#endif // CONFIG_DATA_H
