/**
 * localization.h - Multi-language support
 *
 * Supports: Spanish (ES), English (EN)
 */

#ifndef LOCALIZATION_H
#define LOCALIZATION_H

// ============================================================================
// Language enum
// ============================================================================

enum Language {
    LANG_ES = 0,  // Spanish (default)
    LANG_EN = 1   // English
};

static Language currentLanguage = LANG_ES;

// ============================================================================
// String IDs
// ============================================================================

enum StringID {
    // General
    STR_APP_NAME,
    STR_NEXT,
    STR_BACK,
    STR_SKIP,
    STR_FINISH,
    STR_START,
    STR_CANCEL,
    STR_SAVE,
    STR_SEARCH,
    STR_SCAN,
    STR_ADD,
    STR_CONTINUE,

    // Language selection
    STR_SELECT_LANGUAGE,
    STR_LANGUAGE_ES,
    STR_LANGUAGE_EN,

    // Welcome
    STR_WELCOME,
    STR_WELCOME_DESC,
    STR_BEGIN,

    // WiFi
    STR_WIFI_TITLE,
    STR_WIFI_SELECT,
    STR_WIFI_PASSWORD,
    STR_WIFI_NO_NETWORKS,
    STR_WIFI_SCANNING,
    STR_WIFI_CONFIGURED,
    STR_WIFI_ADD_MORE,
    STR_WIFI_ADD_ANOTHER,
    STR_WIFI_NETWORKS_CONFIGURED,
    STR_WIFI_SHOW_PASSWORD,

    // API Key
    STR_API_KEY_TITLE,
    STR_API_KEY_DESC,
    STR_API_KEY_PLACEHOLDER,

    // Location
    STR_LOCATION_TITLE,
    STR_LOCATION_DESC,
    STR_LOCATION_SEARCH,
    STR_LOCATION_SEARCH_PLACEHOLDER,
    STR_LOCATION_MANUAL,
    STR_LOCATION_LATITUDE,
    STR_LOCATION_LONGITUDE,
    STR_LOCATION_NO_RESULTS,
    STR_LOCATION_SEARCH_HINT,

    // Display
    STR_DISPLAY_TITLE,
    STR_DISPLAY_DESC,
    STR_DISPLAY_BRIGHTNESS,
    STR_DISPLAY_UPDATE_INTERVAL,
    STR_DISPLAY_UNITS,
    STR_DISPLAY_UNITS_METRIC,
    STR_DISPLAY_UNITS_IMPERIAL,
    STR_DISPLAY_TIME_FORMAT,
    STR_DISPLAY_24H,
    STR_DISPLAY_12H,

    // Complete
    STR_COMPLETE_TITLE,
    STR_COMPLETE_MSG,
    STR_COMPLETE_SUMMARY_WIFI,
    STR_COMPLETE_SUMMARY_LOCATION,
    STR_COMPLETE_SUMMARY_UPDATE,
    STR_COMPLETE_SUMMARY_UNITS,

    // Units
    STR_MINUTES,
    STR_METRIC,
    STR_IMPERIAL,

    STR_COUNT  // Keep last
};

// ============================================================================
// String tables
// ============================================================================

static const char* strings_es[] = {
    // General
    "Estacion Clima",           // STR_APP_NAME
    "Siguiente",                // STR_NEXT
    "Atras",                    // STR_BACK
    "Omitir",                   // STR_SKIP
    "Finalizar",                // STR_FINISH
    "Iniciar",                  // STR_START
    "Cancelar",                 // STR_CANCEL
    "Guardar",                  // STR_SAVE
    "Buscar",                   // STR_SEARCH
    "Escanear",                 // STR_SCAN
    "Agregar",                  // STR_ADD
    "Continuar",                // STR_CONTINUE

    // Language selection
    "Selecciona idioma",        // STR_SELECT_LANGUAGE
    "Espanol",                  // STR_LANGUAGE_ES
    "English",                  // STR_LANGUAGE_EN

    // Welcome
    "Bienvenido",               // STR_WELCOME
    "Configuremos tu estacion meteorologica.\nEsto tomara solo unos minutos.", // STR_WELCOME_DESC
    "Comenzar",                 // STR_BEGIN

    // WiFi
    "Red WiFi",                 // STR_WIFI_TITLE
    "Selecciona tu red",        // STR_WIFI_SELECT
    "Contrasena WiFi",          // STR_WIFI_PASSWORD
    "No se encontraron redes",  // STR_WIFI_NO_NETWORKS
    "Escaneando...",            // STR_WIFI_SCANNING
    "WiFi Configurado",         // STR_WIFI_CONFIGURED
    "Agregar otra red?",        // STR_WIFI_ADD_MORE
    "Agregar otra red",         // STR_WIFI_ADD_ANOTHER
    "Redes configuradas: %d de 3", // STR_WIFI_NETWORKS_CONFIGURED
    "Mostrar contrasena",       // STR_WIFI_SHOW_PASSWORD

    // API Key
    "API Key",                  // STR_API_KEY_TITLE
    "Ingresa tu API Key de weatherapi.com\nObten una gratis en: weatherapi.com", // STR_API_KEY_DESC
    "API Key (32 caracteres)",  // STR_API_KEY_PLACEHOLDER

    // Location
    "Ubicacion",                // STR_LOCATION_TITLE
    "Donde te encuentras?",     // STR_LOCATION_DESC
    "Buscar ciudad",            // STR_LOCATION_SEARCH
    "Buscar ciudad...",         // STR_LOCATION_SEARCH_PLACEHOLDER
    "Coordenadas",              // STR_LOCATION_MANUAL
    "Latitud",                  // STR_LOCATION_LATITUDE
    "Longitud",                 // STR_LOCATION_LONGITUDE
    "No se encontraron resultados", // STR_LOCATION_NO_RESULTS
    "Escribe el nombre de tu ciudad y presiona Buscar", // STR_LOCATION_SEARCH_HINT

    // Display
    "Pantalla",                 // STR_DISPLAY_TITLE
    "Ajustes de visualizacion", // STR_DISPLAY_DESC
    "Brillo",                   // STR_DISPLAY_BRIGHTNESS
    "Actualizar cada",          // STR_DISPLAY_UPDATE_INTERVAL
    "Unidades",                 // STR_DISPLAY_UNITS
    "Metrico (C, km/h)",        // STR_DISPLAY_UNITS_METRIC
    "Imperial (F, mph)",        // STR_DISPLAY_UNITS_IMPERIAL
    "Formato hora",             // STR_DISPLAY_TIME_FORMAT
    "24 horas",                 // STR_DISPLAY_24H
    "12 horas (AM/PM)",         // STR_DISPLAY_12H

    // Complete
    "Listo!",                   // STR_COMPLETE_TITLE
    "Tu estacion esta configurada!", // STR_COMPLETE_MSG
    "WiFi",                     // STR_COMPLETE_SUMMARY_WIFI
    "Ubicacion",                // STR_COMPLETE_SUMMARY_LOCATION
    "Actualizacion",            // STR_COMPLETE_SUMMARY_UPDATE
    "Unidades",                 // STR_COMPLETE_SUMMARY_UNITS

    // Units
    "min",                      // STR_MINUTES
    "Metrico",                  // STR_METRIC
    "Imperial",                 // STR_IMPERIAL
};

static const char* strings_en[] = {
    // General
    "Weather Station",          // STR_APP_NAME
    "Next",                     // STR_NEXT
    "Back",                     // STR_BACK
    "Skip",                     // STR_SKIP
    "Finish",                   // STR_FINISH
    "Start",                    // STR_START
    "Cancel",                   // STR_CANCEL
    "Save",                     // STR_SAVE
    "Search",                   // STR_SEARCH
    "Scan",                     // STR_SCAN
    "Add",                      // STR_ADD
    "Continue",                 // STR_CONTINUE

    // Language selection
    "Select language",          // STR_SELECT_LANGUAGE
    "Espanol",                  // STR_LANGUAGE_ES
    "English",                  // STR_LANGUAGE_EN

    // Welcome
    "Welcome",                  // STR_WELCOME
    "Let's set up your weather station.\nThis will only take a few minutes.", // STR_WELCOME_DESC
    "Begin",                    // STR_BEGIN

    // WiFi
    "WiFi Network",             // STR_WIFI_TITLE
    "Select your network",      // STR_WIFI_SELECT
    "WiFi Password",            // STR_WIFI_PASSWORD
    "No networks found",        // STR_WIFI_NO_NETWORKS
    "Scanning...",              // STR_WIFI_SCANNING
    "WiFi Configured",          // STR_WIFI_CONFIGURED
    "Add another network?",     // STR_WIFI_ADD_MORE
    "Add another network",      // STR_WIFI_ADD_ANOTHER
    "Networks configured: %d of 3", // STR_WIFI_NETWORKS_CONFIGURED
    "Show password",            // STR_WIFI_SHOW_PASSWORD

    // API Key
    "API Key",                  // STR_API_KEY_TITLE
    "Enter your weatherapi.com API Key\nGet one free at: weatherapi.com", // STR_API_KEY_DESC
    "API Key (32 characters)",  // STR_API_KEY_PLACEHOLDER

    // Location
    "Location",                 // STR_LOCATION_TITLE
    "Where are you located?",   // STR_LOCATION_DESC
    "Search city",              // STR_LOCATION_SEARCH
    "Search city...",           // STR_LOCATION_SEARCH_PLACEHOLDER
    "Coordinates",              // STR_LOCATION_MANUAL
    "Latitude",                 // STR_LOCATION_LATITUDE
    "Longitude",                // STR_LOCATION_LONGITUDE
    "No results found",         // STR_LOCATION_NO_RESULTS
    "Type your city name and press Search", // STR_LOCATION_SEARCH_HINT

    // Display
    "Display",                  // STR_DISPLAY_TITLE
    "Display settings",         // STR_DISPLAY_DESC
    "Brightness",               // STR_DISPLAY_BRIGHTNESS
    "Update every",             // STR_DISPLAY_UPDATE_INTERVAL
    "Units",                    // STR_DISPLAY_UNITS
    "Metric (C, km/h)",         // STR_DISPLAY_UNITS_METRIC
    "Imperial (F, mph)",        // STR_DISPLAY_UNITS_IMPERIAL
    "Time format",              // STR_DISPLAY_TIME_FORMAT
    "24 hours",                 // STR_DISPLAY_24H
    "12 hours (AM/PM)",         // STR_DISPLAY_12H

    // Complete
    "Done!",                    // STR_COMPLETE_TITLE
    "Your station is configured!", // STR_COMPLETE_MSG
    "WiFi",                     // STR_COMPLETE_SUMMARY_WIFI
    "Location",                 // STR_COMPLETE_SUMMARY_LOCATION
    "Update",                   // STR_COMPLETE_SUMMARY_UPDATE
    "Units",                    // STR_COMPLETE_SUMMARY_UNITS

    // Units
    "min",                      // STR_MINUTES
    "Metric",                   // STR_METRIC
    "Imperial",                 // STR_IMPERIAL
};

// ============================================================================
// Get localized string
// ============================================================================

inline const char* tr(StringID id) {
    if (id >= STR_COUNT) return "???";

    switch (currentLanguage) {
        case LANG_EN:
            return strings_en[id];
        case LANG_ES:
        default:
            return strings_es[id];
    }
}

// ============================================================================
// Set language
// ============================================================================

inline void setLanguage(Language lang) {
    currentLanguage = lang;
}

inline Language getLanguage() {
    return currentLanguage;
}

#endif // LOCALIZATION_H
