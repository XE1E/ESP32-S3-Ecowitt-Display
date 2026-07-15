/**
 * weather_icons.h - Definiciones de iconos del clima
 *
 * Fuente: erikflowers/weather-icons
 * Fuentes convertidas: weather_icons_32.c, weather_icons_48.c
 */

#ifndef WEATHER_ICONS_H
#define WEATHER_ICONS_H

#include <lvgl.h>

// ============================================================================
// Declaracion de fuentes convertidas
// ============================================================================

LV_FONT_DECLARE(weather_icons_32);
LV_FONT_DECLARE(weather_icons_48);

// ============================================================================
// Habilitar iconos de clima (comentar para usar placeholders de texto)
// ============================================================================

#define WEATHER_ICONS_FONT_AVAILABLE

#ifdef WEATHER_ICONS_FONT_AVAILABLE

// ============================================================================
// Iconos con fuente Weather Icons (UTF-8 encoded)
// ============================================================================

// Condiciones principales
#define WI_DAY_SUNNY        "\xEF\x80\x8D"  // U+F00D
#define WI_NIGHT_CLEAR      "\xEF\x80\xAE"  // U+F02E
#define WI_DAY_CLOUDY       "\xEF\x80\x82"  // U+F002
#define WI_NIGHT_CLOUDY     "\xEF\x82\x86"  // U+F086
#define WI_CLOUDY           "\xEF\x80\x93"  // U+F013
#define WI_RAIN             "\xEF\x80\x99"  // U+F019
#define WI_THUNDERSTORM     "\xEF\x80\x9E"  // U+F01E
#define WI_SNOW             "\xEF\x80\x9B"  // U+F01B
#define WI_FOG              "\xEF\x80\x94"  // U+F014
#define WI_STRONG_WIND      "\xEF\x81\x90"  // U+F050
#define WI_DAY_RAIN         "\xEF\x80\x88"  // U+F008
#define WI_DAY_THUNDERSTORM "\xEF\x80\x90"  // U+F010

// Indicadores
#define WI_THERMOMETER      "\xEF\x81\x95"  // U+F055
#define WI_HUMIDITY         "\xEF\x81\xBA"  // U+F07A
#define WI_BAROMETER        "\xEF\x81\xB9"  // U+F079
#define WI_SUNRISE          "\xEF\x81\x91"  // U+F051
#define WI_SUNSET           "\xEF\x81\x92"  // U+F052
#define WI_UMBRELLA         "\xEF\x82\x84"  // U+F084
#define WI_HOT              "\xEF\x81\xB2"  // U+F072
#define WI_COLD             "\xEF\x81\xB6"  // U+F076

// Fases lunares
#define WI_MOON_NEW         "\xEF\x82\x95"  // U+F095
#define WI_MOON_WAXING_CRES "\xEF\x83\x92"  // U+F0D2
#define WI_MOON_FIRST_QTR   "\xEF\x82\x9C"  // U+F09C
#define WI_MOON_WAXING_GIB  "\xEF\x83\x95"  // U+F0D5
#define WI_MOON_FULL        "\xEF\x82\xA3"  // U+F0A3
#define WI_MOON_WANING_GIB  "\xEF\x83\x98"  // U+F0D8
#define WI_MOON_THIRD_QTR   "\xEF\x82\xAA"  // U+F0AA
#define WI_MOON_WANING_CRES "\xEF\x83\x9B"  // U+F0DB

// Direcciones del viento
#define WI_DIR_N            "\xEF\x81\x98"  // U+F058
#define WI_DIR_NE           "\xEF\x81\x97"  // U+F057
#define WI_DIR_E            "\xEF\x81\x8D"  // U+F04D
#define WI_DIR_SE           "\xEF\x81\x84"  // U+F044
#define WI_DIR_S            "\xEF\x81\x84"  // U+F044
#define WI_DIR_SW           "\xEF\x81\x83"  // U+F043
#define WI_DIR_W            "\xEF\x81\x88"  // U+F048
#define WI_DIR_NW           "\xEF\x81\x96"  // U+F056

#else

// ============================================================================
// Placeholders de texto (mientras no tengamos la fuente)
// ============================================================================

// Condiciones principales
#define WI_DAY_SUNNY        "Sol"
#define WI_NIGHT_CLEAR      "Luna"
#define WI_DAY_CLOUDY       "Nub"
#define WI_NIGHT_CLOUDY     "Nub"
#define WI_CLOUDY           "Nub"
#define WI_RAIN             "Lluv"
#define WI_THUNDERSTORM     "Torm"
#define WI_SNOW             "Niev"
#define WI_FOG              "Nieb"
#define WI_STRONG_WIND      "Vien"
#define WI_DAY_RAIN         "Lluv"
#define WI_DAY_THUNDERSTORM "Torm"

// Indicadores
#define WI_THERMOMETER      "T"
#define WI_HUMIDITY         "H"
#define WI_BAROMETER        "P"
#define WI_SUNRISE          "Sale"
#define WI_SUNSET           "Pone"
#define WI_UMBRELLA         "Lluv"
#define WI_HOT              "Cal"
#define WI_COLD             "Frio"

// Fases lunares
#define WI_MOON_NEW         "LN"
#define WI_MOON_WAXING_CRES "LC"
#define WI_MOON_FIRST_QTR   "C+"
#define WI_MOON_WAXING_GIB  "LG"
#define WI_MOON_FULL        "LL"
#define WI_MOON_WANING_GIB  "MG"
#define WI_MOON_THIRD_QTR   "C-"
#define WI_MOON_WANING_CRES "MC"

// Direcciones del viento
#define WI_DIR_N            "N"
#define WI_DIR_NE           "NE"
#define WI_DIR_E            "E"
#define WI_DIR_SE           "SE"
#define WI_DIR_S            "S"
#define WI_DIR_SW           "SO"
#define WI_DIR_W            "O"
#define WI_DIR_NW           "NO"

#endif // WEATHER_ICONS_FONT_AVAILABLE

// ============================================================================
// Funciones de mapeo
// ============================================================================

/**
 * Obtener icono segun condicion del clima
 * @param condition Codigo de condicion (ej: "sunny", "cloudy", "rain")
 * @param is_day true si es de dia, false si es de noche
 * @return String con el icono correspondiente
 */
inline const char* getWeatherIcon(const char* condition, bool is_day) {
    if (!condition) return WI_CLOUDY;

    // Despejado
    if (strstr(condition, "clear") || strstr(condition, "sunny")) {
        return is_day ? WI_DAY_SUNNY : WI_NIGHT_CLEAR;
    }
    // Parcialmente nublado
    if (strstr(condition, "partly") || strstr(condition, "few")) {
        return is_day ? WI_DAY_CLOUDY : WI_NIGHT_CLOUDY;
    }
    // Nublado
    if (strstr(condition, "cloud") || strstr(condition, "overcast")) {
        return WI_CLOUDY;
    }
    // Tormenta
    if (strstr(condition, "thunder") || strstr(condition, "storm")) {
        return WI_THUNDERSTORM;
    }
    // Lluvia
    if (strstr(condition, "rain") || strstr(condition, "drizzle") || strstr(condition, "shower")) {
        return is_day ? WI_DAY_RAIN : WI_RAIN;
    }
    // Nieve
    if (strstr(condition, "snow") || strstr(condition, "sleet") || strstr(condition, "ice")) {
        return WI_SNOW;
    }
    // Niebla
    if (strstr(condition, "fog") || strstr(condition, "mist") || strstr(condition, "haze")) {
        return WI_FOG;
    }
    // Viento
    if (strstr(condition, "wind")) {
        return WI_STRONG_WIND;
    }

    // Default
    return WI_CLOUDY;
}

/**
 * Obtener icono de fase lunar
 * @param illumination Porcentaje de iluminacion (0-100)
 * @param is_waxing true si esta creciendo
 * @return String con el icono de la fase
 */
inline const char* getMoonPhaseIcon(int illumination, bool is_waxing) {
    if (illumination < 5) return WI_MOON_NEW;
    if (illumination < 35) return is_waxing ? WI_MOON_WAXING_CRES : WI_MOON_WANING_CRES;
    if (illumination < 65) return is_waxing ? WI_MOON_FIRST_QTR : WI_MOON_THIRD_QTR;
    if (illumination < 95) return is_waxing ? WI_MOON_WAXING_GIB : WI_MOON_WANING_GIB;
    return WI_MOON_FULL;
}

/**
 * Obtener icono de direccion del viento
 * @param degrees Grados (0-360, 0=N)
 * @return String con el icono de direccion
 */
inline const char* getWindDirectionIcon(int degrees) {
    // Normalizar a 0-360
    degrees = ((degrees % 360) + 360) % 360;

    if (degrees < 23 || degrees >= 338) return WI_DIR_N;
    if (degrees < 68) return WI_DIR_NE;
    if (degrees < 113) return WI_DIR_E;
    if (degrees < 158) return WI_DIR_SE;
    if (degrees < 203) return WI_DIR_S;
    if (degrees < 248) return WI_DIR_SW;
    if (degrees < 293) return WI_DIR_W;
    return WI_DIR_NW;
}

#endif // WEATHER_ICONS_H
