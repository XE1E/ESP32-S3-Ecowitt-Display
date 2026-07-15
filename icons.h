/**
 * icons.h - Definiciones unificadas de iconos
 *
 * Combina:
 * - Simbolos nativos de LVGL (UI, navegacion, estado)
 * - Weather Icons (clima) - requiere fuente adicional
 */

#ifndef ICONS_H
#define ICONS_H

#include <lvgl.h>

// ============================================================================
// SIMBOLOS LVGL NATIVOS (siempre disponibles)
// ============================================================================

// --- Navegacion ---
#define ICON_LEFT           LV_SYMBOL_LEFT
#define ICON_RIGHT          LV_SYMBOL_RIGHT
#define ICON_UP             LV_SYMBOL_UP
#define ICON_DOWN           LV_SYMBOL_DOWN
#define ICON_OK             LV_SYMBOL_OK
#define ICON_CLOSE          LV_SYMBOL_CLOSE
#define ICON_REFRESH        LV_SYMBOL_REFRESH
#define ICON_HOME           LV_SYMBOL_HOME
#define ICON_PLUS           LV_SYMBOL_PLUS
#define ICON_MINUS          LV_SYMBOL_MINUS

// --- Configuracion ---
#define ICON_SETTINGS       LV_SYMBOL_SETTINGS
#define ICON_EDIT           LV_SYMBOL_EDIT
#define ICON_SAVE           LV_SYMBOL_SAVE
#define ICON_TRASH          LV_SYMBOL_TRASH
#define ICON_LIST           LV_SYMBOL_LIST

// --- Estado ---
#define ICON_WIFI           LV_SYMBOL_WIFI
#define ICON_WARNING        LV_SYMBOL_WARNING
#define ICON_BELL           LV_SYMBOL_BELL
#define ICON_POWER          LV_SYMBOL_POWER
#define ICON_CHARGE         LV_SYMBOL_CHARGE

// --- Bateria ---
#define ICON_BATTERY_FULL   LV_SYMBOL_BATTERY_FULL
#define ICON_BATTERY_3      LV_SYMBOL_BATTERY_3
#define ICON_BATTERY_2      LV_SYMBOL_BATTERY_2
#define ICON_BATTERY_1      LV_SYMBOL_BATTERY_1
#define ICON_BATTERY_EMPTY  LV_SYMBOL_BATTERY_EMPTY

// --- Visibilidad ---
#define ICON_EYE_OPEN       LV_SYMBOL_EYE_OPEN
#define ICON_EYE_CLOSE      LV_SYMBOL_EYE_CLOSE

// --- Multimedia ---
#define ICON_PLAY           LV_SYMBOL_PLAY
#define ICON_PAUSE          LV_SYMBOL_PAUSE
#define ICON_STOP           LV_SYMBOL_STOP
#define ICON_PREV           LV_SYMBOL_PREV
#define ICON_NEXT           LV_SYMBOL_NEXT

// --- Teclado ---
#define ICON_KEYBOARD       LV_SYMBOL_KEYBOARD
#define ICON_BACKSPACE      LV_SYMBOL_BACKSPACE
#define ICON_NEW_LINE       LV_SYMBOL_NEW_LINE

// ============================================================================
// ICONOS DE CLIMA (weather_icons.h)
// ============================================================================

#include "weather_icons.h"

// Alias mas cortos para uso comun
#define ICON_SUNNY          WI_DAY_SUNNY
#define ICON_NIGHT          WI_NIGHT_CLEAR
#define ICON_CLOUDY         WI_CLOUDY
#define ICON_RAIN           WI_RAIN
#define ICON_STORM          WI_THUNDERSTORM
#define ICON_SNOW           WI_SNOW
#define ICON_FOG            WI_FOG
#define ICON_WIND           WI_STRONG_WIND
#define ICON_SUNRISE        WI_SUNRISE
#define ICON_SUNSET         WI_SUNSET
#define ICON_THERMOMETER    WI_THERMOMETER
#define ICON_HUMIDITY       WI_HUMIDITY
#define ICON_BAROMETER      WI_BAROMETER

// ============================================================================
// INDICADORES DE COLOR (caracteres simples)
// ============================================================================

// Indicadores visuales para titulos de paneles
#define INDICATOR_BAR       "|"     // Barra vertical de color
#define INDICATOR_DOT       "o"     // Punto/circulo
#define INDICATOR_BULLET    "*"     // Asterisco/bullet

// ============================================================================
// HELPERS
// ============================================================================

/**
 * Obtener icono de bateria segun porcentaje
 */
inline const char* getBatteryIcon(int percent) {
    if (percent >= 87) return ICON_BATTERY_FULL;
    if (percent >= 62) return ICON_BATTERY_3;
    if (percent >= 37) return ICON_BATTERY_2;
    if (percent >= 12) return ICON_BATTERY_1;
    return ICON_BATTERY_EMPTY;
}

/**
 * Obtener icono de WiFi segun RSSI
 * (LVGL solo tiene un icono de WiFi, pero podemos cambiar color)
 */
inline const char* getWifiIcon(int rssi) {
    (void)rssi;  // Ignorado, LVGL solo tiene un icono
    return ICON_WIFI;
}

/**
 * Obtener color de WiFi segun RSSI
 */
inline lv_color_t getWifiColor(int rssi) {
    if (rssi >= -50) return lv_color_hex(0x4CAF50);  // Verde - Excelente
    if (rssi >= -60) return lv_color_hex(0x8BC34A);  // Verde claro - Bueno
    if (rssi >= -70) return lv_color_hex(0xFFEB3B);  // Amarillo - Regular
    if (rssi >= -80) return lv_color_hex(0xFF9800);  // Naranja - Debil
    return lv_color_hex(0xF44336);                   // Rojo - Muy debil
}

#endif // ICONS_H
