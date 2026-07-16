/**
 * ui_dashboard.h - Pantalla principal del dashboard
 *
 * Layout 1024x600 para datos de estación Ecowitt
 */

#ifndef UI_DASHBOARD_H
#define UI_DASHBOARD_H

#include <Arduino.h>
#include <lvgl.h>
#include <time.h>
#include "config.h"
#include "preferences_manager.h"
#include "weather_icons.h"

// ============================================================================
// Sistema de Temas Mejorado
// ============================================================================

struct ThemeColors {
    lv_color_t bg;
    lv_color_t card;
    lv_color_t border;
    lv_color_t text_primary;
    lv_color_t text_secondary;
    lv_color_t text_muted;
    lv_color_t header_bg;
    lv_color_t accent;
};

// Tema claro - colores suaves y profesionales
static const ThemeColors THEME_LIGHT = {
    .bg = LV_COLOR_MAKE(0xF0, 0xF2, 0xF5),
    .card = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF),
    .border = LV_COLOR_MAKE(0xE0, 0xE0, 0xE0),
    .text_primary = LV_COLOR_MAKE(0x1A, 0x1A, 0x1A),
    .text_secondary = LV_COLOR_MAKE(0x55, 0x55, 0x55),
    .text_muted = LV_COLOR_MAKE(0x88, 0x88, 0x88),
    .header_bg = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF),
    .accent = LV_COLOR_MAKE(0x21, 0x96, 0xF3),
};

// Tema oscuro - estilo servidor (0f1a2a base)
static const ThemeColors THEME_DARK = {
    .bg = LV_COLOR_MAKE(0x0B, 0x11, 0x20),        // Body gradient base
    .card = LV_COLOR_MAKE(0x1A, 0x25, 0x35),      // Card approx 5% white
    .border = LV_COLOR_MAKE(0x2A, 0x35, 0x45),    // Border
    .text_primary = LV_COLOR_MAKE(0xE8, 0xED, 0xF5),   // --ink
    .text_secondary = LV_COLOR_MAKE(0xCB, 0xD5, 0xE1), // slate-300
    .text_muted = LV_COLOR_MAKE(0x84, 0x96, 0xA6),     // --muted
    .header_bg = LV_COLOR_MAKE(0x0A, 0x0F, 0x1C),      // Appbar
    .accent = LV_COLOR_MAKE(0x60, 0xA5, 0xFA),         // blue-400
};

// Estado actual del tema (se carga de NVS)
static bool darkMode = false;
static bool useCelsius = true;

// Cargar preferencias de UI desde NVS
void loadUIPreferences() {
    UserPreferences* prefs = getPreferences();
    darkMode = prefs->dark_mode;
    useCelsius = prefs->use_celsius;
}

// Guardar preferencias de UI a NVS
void saveUIPreferences() {
    UserPreferences* prefs = getPreferences();
    prefs->dark_mode = darkMode;
    prefs->use_celsius = useCelsius;
    savePreferences();
}

// Colores dinamicos segun tema actual
#define COLOR_BG            (darkMode ? THEME_DARK.bg : THEME_LIGHT.bg)
#define COLOR_CARD          (darkMode ? THEME_DARK.card : THEME_LIGHT.card)
#define COLOR_BORDER        (darkMode ? THEME_DARK.border : THEME_LIGHT.border)
#define COLOR_TEXT_PRIMARY  (darkMode ? THEME_DARK.text_primary : THEME_LIGHT.text_primary)
#define COLOR_TEXT_SECONDARY (darkMode ? THEME_DARK.text_secondary : THEME_LIGHT.text_secondary)
#define COLOR_TEXT_MUTED    (darkMode ? THEME_DARK.text_muted : THEME_LIGHT.text_muted)
#define COLOR_HEADER        (darkMode ? THEME_DARK.header_bg : THEME_LIGHT.header_bg)
#define COLOR_ACCENT        (darkMode ? THEME_DARK.accent : THEME_LIGHT.accent)

// Colores de estado - estilo servidor
#define COLOR_GREEN         lv_color_hex(0x6EE7B7)  // emerald-300
#define COLOR_YELLOW        lv_color_hex(0xFDE047)  // yellow-300
#define COLOR_RED           lv_color_hex(0xF87171)  // red-400
#define COLOR_CYAN          lv_color_hex(0x67E8F9)  // cyan-300 (humidity)
#define COLOR_VIOLET        lv_color_hex(0xC4B5FD)  // violet-300 (pressure)
#define COLOR_TEMP_HOT      lv_color_hex(0xFDBA74)  // orange-300
#define COLOR_TEMP_COLD     lv_color_hex(0x7DD3FC)  // sky-300
#define COLOR_WIND          lv_color_hex(0x6EE7B7)  // emerald-300
#define COLOR_RAIN          lv_color_hex(0x93C5FD)  // blue-300
#define COLOR_SUN           lv_color_hex(0xFFB800)  // weather-sun
#define COLOR_UV            lv_color_hex(0xFCD34D)  // amber-300

// Iconos usando simbolos LVGL
#define ICON_WIFI       LV_SYMBOL_WIFI
#define ICON_SETTINGS   LV_SYMBOL_SETTINGS
#define ICON_WARNING    LV_SYMBOL_WARNING
#define ICON_OK         LV_SYMBOL_OK
#define ICON_REFRESH    LV_SYMBOL_REFRESH
#define ICON_UP         LV_SYMBOL_UP
#define ICON_DOWN       LV_SYMBOL_DOWN

// Conversion de temperatura
float toDisplayTemp(float celsius) {
    return useCelsius ? celsius : (celsius * 9.0f / 5.0f + 32.0f);
}

const char* tempUnit() {
    return useCelsius ? "C" : "F";
}

// Dimensiones
#define HEADER_HEIGHT       50
#define FOOTER_HEIGHT       50
#define CARD_RADIUS         16
#define CARD_PADDING        12
#define GAP                 10

// ============================================================================
// Referencias a elementos UI
// ============================================================================

// Contenedores principales
static lv_obj_t *scr_dashboard = nullptr;
static lv_obj_t *header = nullptr;
static lv_obj_t *footer = nullptr;
static lv_obj_t *content_area = nullptr;

// Header elements
static lv_obj_t *lbl_status_icon = nullptr;
static lv_obj_t *lbl_status_text = nullptr;
static lv_obj_t *lbl_wifi_rssi = nullptr;
lv_obj_t *btn_settings = nullptr;
static lv_obj_t *lbl_header_time = nullptr;

// Panel Reloj
static lv_obj_t *panel_clock = nullptr;
static lv_obj_t *lbl_clock_time = nullptr;
static lv_obj_t *lbl_clock_date = nullptr;

// Panel Presion
static lv_obj_t *panel_pressure = nullptr;
static lv_obj_t *lbl_pressure_value = nullptr;
static lv_obj_t *lbl_pressure_trend = nullptr;
static lv_obj_t *lbl_pressure_diff = nullptr;

// Panel Alertas/Tendencias
static lv_obj_t *panel_alerts = nullptr;
static lv_obj_t *lbl_alert_status = nullptr;
static lv_obj_t *lbl_trend_temp = nullptr;
static lv_obj_t *lbl_trend_pressure = nullptr;
static lv_obj_t *lbl_trend_humidity = nullptr;

// Panel Temperatura
static lv_obj_t *panel_temp = nullptr;
static lv_obj_t *lbl_temp_value = nullptr;
static lv_obj_t *lbl_temp_feels = nullptr;
static lv_obj_t *lbl_temp_max = nullptr;
static lv_obj_t *lbl_temp_min = nullptr;

// Panel Humedad
static lv_obj_t *panel_humidity = nullptr;
static lv_obj_t *arc_humidity = nullptr;
static lv_obj_t *lbl_humidity_value = nullptr;
static lv_obj_t *lbl_humidity_label = nullptr;

// Panel Condiciones
static lv_obj_t *panel_conditions = nullptr;
static lv_obj_t *lbl_condition_icon = nullptr;
static lv_obj_t *lbl_condition_text = nullptr;
static lv_obj_t *lbl_wind = nullptr;
static lv_obj_t *lbl_wind_gust = nullptr;
static lv_obj_t *lbl_uv = nullptr;
static lv_obj_t *lbl_rain = nullptr;
static lv_obj_t *lbl_dewpoint = nullptr;

// Panel Pronóstico (5 dias)
static lv_obj_t *panel_forecast = nullptr;
static lv_obj_t *forecast_cards[5] = {nullptr};
static lv_obj_t *lbl_fc_day[5] = {nullptr};
static lv_obj_t *lbl_fc_icon[5] = {nullptr};
static lv_obj_t *lbl_fc_temp[5] = {nullptr};
static lv_obj_t *lbl_fc_rain[5] = {nullptr};

// Panel Interior
static lv_obj_t *panel_indoor = nullptr;
static lv_obj_t *lbl_indoor_title = nullptr;
static lv_obj_t *lbl_indoor_temp = nullptr;
static lv_obj_t *lbl_indoor_humidity = nullptr;
static lv_obj_t *lbl_indoor_pressure = nullptr;
static lv_obj_t *lbl_indoor_diff = nullptr;

// Footer elements
static lv_obj_t *lbl_footer_uv = nullptr;
static lv_obj_t *lbl_footer_aqi = nullptr;
static lv_obj_t *lbl_footer_sunrise = nullptr;
static lv_obj_t *lbl_footer_sunset = nullptr;
static lv_obj_t *lbl_footer_moon = nullptr;

// ============================================================================
// Estilos
// ============================================================================

static lv_style_t style_card;
static lv_style_t style_header;
static lv_style_t style_footer;
static bool styles_initialized = false;

void initDashboardStyles() {
    if (styles_initialized) return;

    // Estilo de tarjeta
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, COLOR_CARD);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_radius(&style_card, CARD_RADIUS);
    lv_style_set_border_width(&style_card, 1);
    lv_style_set_border_color(&style_card, COLOR_BORDER);
    lv_style_set_shadow_width(&style_card, 12);
    lv_style_set_shadow_color(&style_card, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&style_card, LV_OPA_10);
    lv_style_set_shadow_ofs_y(&style_card, 3);
    lv_style_set_pad_all(&style_card, CARD_PADDING);

    // Estilo header
    lv_style_init(&style_header);
    lv_style_set_bg_color(&style_header, COLOR_CARD);
    lv_style_set_bg_opa(&style_header, LV_OPA_COVER);
    lv_style_set_border_width(&style_header, 0);
    lv_style_set_border_side(&style_header, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_color(&style_header, COLOR_BORDER);
    lv_style_set_radius(&style_header, 0);
    lv_style_set_pad_hor(&style_header, 15);
    lv_style_set_pad_ver(&style_header, 8);

    // Estilo footer
    lv_style_init(&style_footer);
    lv_style_set_bg_color(&style_footer, lv_color_hex(0x2C3E50));
    lv_style_set_bg_opa(&style_footer, LV_OPA_COVER);
    lv_style_set_radius(&style_footer, 0);
    lv_style_set_pad_hor(&style_footer, 20);
    lv_style_set_pad_ver(&style_footer, 10);

    styles_initialized = true;
}

// ============================================================================
// Crear tarjeta (panel)
// ============================================================================

lv_obj_t* createCard(lv_obj_t *parent, int x, int y, int w, int h) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    return card;
}

// ============================================================================
// Crear Header
// ============================================================================

// Forward declaration para refresh del dashboard
void refreshDashboardTheme();

// Elementos adicionales del header
static lv_obj_t *lbl_server_status = nullptr;
static lv_obj_t *btn_theme = nullptr;
static lv_obj_t *btn_unit = nullptr;

void createHeader(lv_obj_t *parent) {
    header = lv_obj_create(parent);
    lv_obj_set_size(header, SCREEN_WIDTH, HEADER_HEIGHT);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_add_style(header, &style_header, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    // === Lado izquierdo: Nombre estacion ===
    lv_obj_t *station_name = lv_label_create(header);
    lv_label_set_text(station_name, "Estacion Clima XE1E");
    lv_obj_set_style_text_color(station_name, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(station_name, &lv_font_montserrat_16, 0);
    lv_obj_align(station_name, LV_ALIGN_LEFT_MID, 10, -8);

    lv_obj_t *station_loc = lv_label_create(header);
    lv_label_set_text(station_loc, "Ciudad de Mexico");
    lv_obj_set_style_text_color(station_loc, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(station_loc, &lv_font_montserrat_12, 0);
    lv_obj_align(station_loc, LV_ALIGN_LEFT_MID, 10, 10);

    // Status indicators (pequenos, al lado del nombre)
    lbl_wifi_rssi = lv_label_create(header);
    lv_label_set_text(lbl_wifi_rssi, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(lbl_wifi_rssi, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_wifi_rssi, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_wifi_rssi, LV_ALIGN_LEFT_MID, 200, 0);

    lbl_server_status = lv_label_create(header);
    lv_label_set_text(lbl_server_status, "EN VIVO");
    lv_obj_set_style_text_color(lbl_server_status, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_server_status, &lv_font_montserrat_12, 0);
    lv_obj_align_to(lbl_server_status, lbl_wifi_rssi, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // === Centro: Fecha + Hora grande ===
    lbl_status_text = lv_label_create(header);  // Reutilizado para fecha
    lv_label_set_text(lbl_status_text, "Martes 14 de Julio");
    lv_obj_set_style_text_color(lbl_status_text, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_status_text, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_status_text, LV_ALIGN_CENTER, 0, -10);

    lbl_header_time = lv_label_create(header);
    lv_label_set_text(lbl_header_time, "22:59:21");
    lv_obj_set_style_text_color(lbl_header_time, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_header_time, &lv_font_montserrat_20, 0);
    lv_obj_align(lbl_header_time, LV_ALIGN_CENTER, 0, 10);

    // === Lado derecho: Toggles estilo servidor ===

    // Unit toggle (C . km/h)
    btn_unit = lv_btn_create(header);
    lv_obj_set_size(btn_unit, 70, 28);
    lv_obj_align(btn_unit, LV_ALIGN_RIGHT_MID, -130, 0);
    lv_obj_set_style_bg_color(btn_unit, COLOR_BORDER, 0);
    lv_obj_set_style_radius(btn_unit, 14, 0);
    lv_obj_set_style_shadow_width(btn_unit, 0, 0);

    lv_obj_t *unit_label = lv_label_create(btn_unit);
    lv_label_set_text(unit_label, "C . km/h");
    lv_obj_set_style_text_color(unit_label, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(unit_label, &lv_font_montserrat_12, 0);
    lv_obj_center(unit_label);

    lv_obj_add_event_cb(btn_unit, [](lv_event_t *e) {
        useCelsius = !useCelsius;
        lv_obj_t *btn = lv_event_get_target(e);
        lv_obj_t *lbl = lv_obj_get_child(btn, 0);
        lv_label_set_text(lbl, useCelsius ? "C . km/h" : "F . mph");
        saveUIPreferences();
        extern void updateDashboardWeather();
        updateDashboardWeather();
    }, LV_EVENT_CLICKED, NULL);

    // Theme toggle (luna/sol)
    btn_theme = lv_btn_create(header);
    lv_obj_set_size(btn_theme, 36, 28);
    lv_obj_align(btn_theme, LV_ALIGN_RIGHT_MID, -85, 0);
    lv_obj_set_style_bg_color(btn_theme, COLOR_BORDER, 0);
    lv_obj_set_style_radius(btn_theme, 14, 0);
    lv_obj_set_style_shadow_width(btn_theme, 0, 0);

    lv_obj_t *theme_icon = lv_label_create(btn_theme);
    lv_label_set_text(theme_icon, "D");  // D=Dark, L=Light (sin emoji)
    lv_obj_set_style_text_color(theme_icon, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(theme_icon, &lv_font_montserrat_14, 0);
    lv_obj_center(theme_icon);

    lv_obj_add_event_cb(btn_theme, [](lv_event_t *e) {
        darkMode = !darkMode;
        lv_obj_t *btn = lv_event_get_target(e);
        lv_obj_t *icon = lv_obj_get_child(btn, 0);
        lv_label_set_text(icon, darkMode ? "D" : "L");
        saveUIPreferences();
        refreshDashboardTheme();
    }, LV_EVENT_CLICKED, NULL);

    // Settings button
    btn_settings = lv_btn_create(header);
    lv_obj_set_size(btn_settings, 36, 28);
    lv_obj_align(btn_settings, LV_ALIGN_RIGHT_MID, -40, 0);
    lv_obj_set_style_bg_color(btn_settings, COLOR_BORDER, 0);
    lv_obj_set_style_radius(btn_settings, 14, 0);
    lv_obj_set_style_shadow_width(btn_settings, 0, 0);

    lv_obj_t *settings_icon = lv_label_create(btn_settings);
    lv_label_set_text(settings_icon, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(settings_icon, COLOR_TEXT_SECONDARY, 0);
    lv_obj_center(settings_icon);
}

// ============================================================================
// Crear Footer
// ============================================================================

void createFooter(lv_obj_t *parent) {
    footer = lv_obj_create(parent);
    lv_obj_set_size(footer, SCREEN_WIDTH, FOOTER_HEIGHT);
    lv_obj_set_pos(footer, 0, SCREEN_HEIGHT - FOOTER_HEIGHT);
    lv_obj_add_style(footer, &style_footer, 0);
    lv_obj_clear_flag(footer, LV_OBJ_FLAG_SCROLLABLE);

    // UV
    lbl_footer_uv = lv_label_create(footer);
    lv_label_set_text(lbl_footer_uv, "* UV: 3 Mod");
    lv_obj_set_style_text_color(lbl_footer_uv, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_uv, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_uv, LV_ALIGN_LEFT_MID, 0, 0);

    // AQI
    lbl_footer_aqi = lv_label_create(footer);
    lv_label_set_text(lbl_footer_aqi, "AQI: 42");
    lv_obj_set_style_text_color(lbl_footer_aqi, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_aqi, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_aqi, LV_ALIGN_LEFT_MID, 150, 0);

    // Sunrise
    lbl_footer_sunrise = lv_label_create(footer);
    lv_label_set_text(lbl_footer_sunrise, "Sale: 06:52");
    lv_obj_set_style_text_color(lbl_footer_sunrise, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_sunrise, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_sunrise, LV_ALIGN_CENTER, -120, 0);

    // Sunset
    lbl_footer_sunset = lv_label_create(footer);
    lv_label_set_text(lbl_footer_sunset, "Pone: 20:14");
    lv_obj_set_style_text_color(lbl_footer_sunset, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_sunset, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_sunset, LV_ALIGN_CENTER, 0, 0);

    // Moon
    lbl_footer_moon = lv_label_create(footer);
    lv_label_set_text(lbl_footer_moon, "Luna: 82%");
    lv_obj_set_style_text_color(lbl_footer_moon, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_moon, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_moon, LV_ALIGN_RIGHT_MID, 0, 0);
}

// ============================================================================
// Crear Panel Reloj
// ============================================================================

void createClockPanel(lv_obj_t *parent, int x, int y) {
    panel_clock = createCard(parent, x, y, 190, 120);

    // Hora grande
    lbl_clock_time = lv_label_create(panel_clock);
    lv_label_set_text(lbl_clock_time, "16:48");
    lv_obj_set_style_text_color(lbl_clock_time, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_clock_time, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_clock_time, LV_ALIGN_TOP_MID, 0, 5);

    // Fecha
    lbl_clock_date = lv_label_create(panel_clock);
    lv_label_set_text(lbl_clock_date, "Dom 13 Jul");
    lv_obj_set_style_text_color(lbl_clock_date, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_clock_date, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_clock_date, LV_ALIGN_BOTTOM_MID, 0, -5);
}

// ============================================================================
// Crear Panel Presion
// ============================================================================

void createPressurePanel(lv_obj_t *parent, int x, int y) {
    panel_pressure = createCard(parent, x, y, 325, 220);

    // Indicador + Titulo
    lv_obj_t *indicator = lv_label_create(panel_pressure);
    lv_label_set_text(indicator, "|");
    lv_obj_set_style_text_color(indicator, COLOR_VIOLET, 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_18, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *title = lv_label_create(panel_pressure);
    lv_label_set_text(title, "PRESION");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align_to(title, indicator, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Icono de barometro (Weather Icons)
    lv_obj_t *baro_icon = lv_label_create(panel_pressure);
    lv_label_set_text(baro_icon, WI_BAROMETER);
    lv_obj_set_style_text_font(baro_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(baro_icon, COLOR_VIOLET, 0);
    lv_obj_align(baro_icon, LV_ALIGN_TOP_RIGHT, -10, 30);

    // Valor GRANDE
    lbl_pressure_value = lv_label_create(panel_pressure);
    lv_label_set_text(lbl_pressure_value, "1012.5");
    lv_obj_set_style_text_color(lbl_pressure_value, COLOR_VIOLET, 0);
    lv_obj_set_style_text_font(lbl_pressure_value, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_pressure_value, LV_ALIGN_TOP_LEFT, 10, 35);

    lv_obj_t *unit = lv_label_create(panel_pressure);
    lv_label_set_text(unit, "hPa");
    lv_obj_set_style_text_color(unit, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(unit, &lv_font_montserrat_20, 0);
    lv_obj_align_to(unit, lbl_pressure_value, LV_ALIGN_OUT_RIGHT_BOTTOM, 8, -5);

    // Tendencia con texto
    lv_obj_t *trend_label = lv_label_create(panel_pressure);
    lv_label_set_text(trend_label, "Tendencia:");
    lv_obj_set_style_text_color(trend_label, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(trend_label, &lv_font_montserrat_16, 0);
    lv_obj_align(trend_label, LV_ALIGN_TOP_LEFT, 10, 95);

    lbl_pressure_trend = lv_label_create(panel_pressure);
    lv_label_set_text(lbl_pressure_trend, LV_SYMBOL_UP " Subiendo");
    lv_obj_set_style_text_color(lbl_pressure_trend, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_pressure_trend, &lv_font_montserrat_20, 0);
    lv_obj_align_to(lbl_pressure_trend, trend_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Diferencia vs ayer
    lv_obj_t *diff_label = lv_label_create(panel_pressure);
    lv_label_set_text(diff_label, "Cambio 3h:");
    lv_obj_set_style_text_color(diff_label, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(diff_label, &lv_font_montserrat_16, 0);
    lv_obj_align(diff_label, LV_ALIGN_TOP_LEFT, 10, 130);

    lbl_pressure_diff = lv_label_create(panel_pressure);
    lv_label_set_text(lbl_pressure_diff, "+1.5 hPa");
    lv_obj_set_style_text_color(lbl_pressure_diff, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_pressure_diff, &lv_font_montserrat_20, 0);
    lv_obj_align_to(lbl_pressure_diff, diff_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Pronostico barometrico
    lv_obj_t *forecast = lv_label_create(panel_pressure);
    lv_label_set_text(forecast, "Pronostico: Estable a buen tiempo");
    lv_obj_set_style_text_color(forecast, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(forecast, &lv_font_montserrat_16, 0);
    lv_obj_align(forecast, LV_ALIGN_BOTTOM_LEFT, 10, -5);
}

// ============================================================================
// Crear Panel Alertas/Tendencias
// ============================================================================

void createAlertsPanel(lv_obj_t *parent, int x, int y, int w) {
    panel_alerts = createCard(parent, x, y, w, 120);

    // Estado de alertas
    lbl_alert_status = lv_label_create(panel_alerts);
    lv_label_set_text(lbl_alert_status, "OK - Sin alertas");
    lv_obj_set_style_text_color(lbl_alert_status, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_alert_status, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_alert_status, LV_ALIGN_TOP_LEFT, 0, 0);

    // Tendencia temperatura
    lbl_trend_temp = lv_label_create(panel_alerts);
    lv_label_set_text(lbl_trend_temp, "T: Temp: +0.8°C vs ayer");
    lv_obj_set_style_text_color(lbl_trend_temp, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_trend_temp, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_trend_temp, LV_ALIGN_TOP_LEFT, 0, 35);

    // Tendencia presión
    lbl_trend_pressure = lv_label_create(panel_alerts);
    lv_label_set_text(lbl_trend_pressure, "P: Presion: -2.1 hPa/3h (bajando)");
    lv_obj_set_style_text_color(lbl_trend_pressure, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_trend_pressure, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_trend_pressure, LV_ALIGN_TOP_LEFT, 0, 58);

    // Tendencia humedad
    lbl_trend_humidity = lv_label_create(panel_alerts);
    lv_label_set_text(lbl_trend_humidity, "H: Humedad: +5% vs ayer");
    lv_obj_set_style_text_color(lbl_trend_humidity, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_trend_humidity, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_trend_humidity, LV_ALIGN_TOP_LEFT, 0, 81);
}

// ============================================================================
// Crear Panel Temperatura
// ============================================================================

void createTemperaturePanel(lv_obj_t *parent, int x, int y) {
    panel_temp = createCard(parent, x, y, 325, 240);

    // Indicador + Titulo
    lv_obj_t *indicator = lv_label_create(panel_temp);
    lv_label_set_text(indicator, "|");
    lv_obj_set_style_text_color(indicator, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_18, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *title = lv_label_create(panel_temp);
    lv_label_set_text(title, "TEMPERATURA");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align_to(title, indicator, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Icono de termometro (Weather Icons)
    lv_obj_t *thermo_icon = lv_label_create(panel_temp);
    lv_label_set_text(thermo_icon, WI_THERMOMETER);
    lv_obj_set_style_text_font(thermo_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(thermo_icon, COLOR_CYAN, 0);
    lv_obj_align(thermo_icon, LV_ALIGN_TOP_RIGHT, -10, 30);

    // Temperatura GRANDE
    lbl_temp_value = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_value, "21.5");
    lv_obj_set_style_text_color(lbl_temp_value, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_temp_value, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_temp_value, LV_ALIGN_TOP_LEFT, 10, 35);

    // Unidad
    lv_obj_t *unit = lv_label_create(panel_temp);
    lv_label_set_text(unit, "C");
    lv_obj_set_style_text_color(unit, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(unit, &lv_font_montserrat_24, 0);
    lv_obj_align_to(unit, lbl_temp_value, LV_ALIGN_OUT_RIGHT_TOP, 5, 8);

    // Condicion
    lbl_condition_text = lv_label_create(panel_temp);
    lv_label_set_text(lbl_condition_text, "Despejado");
    lv_obj_set_style_text_color(lbl_condition_text, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_condition_text, &lv_font_montserrat_20, 0);
    lv_obj_align(lbl_condition_text, LV_ALIGN_TOP_LEFT, 10, 95);

    // Sensacion termica
    lbl_temp_feels = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_feels, "Sensacion: 20.5C");
    lv_obj_set_style_text_color(lbl_temp_feels, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_temp_feels, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_temp_feels, LV_ALIGN_TOP_LEFT, 10, 130);

    // Punto de rocio
    lbl_dewpoint = lv_label_create(panel_temp);
    lv_label_set_text(lbl_dewpoint, "Punto de rocio: 14.2C");
    lv_obj_set_style_text_color(lbl_dewpoint, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_dewpoint, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_dewpoint, LV_ALIGN_TOP_LEFT, 10, 155);

    // Max/Min (abajo)
    lbl_temp_max = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_max, LV_SYMBOL_UP " Max: 26.0C");
    lv_obj_set_style_text_color(lbl_temp_max, COLOR_TEMP_HOT, 0);
    lv_obj_set_style_text_font(lbl_temp_max, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_temp_max, LV_ALIGN_BOTTOM_LEFT, 10, -5);

    lbl_temp_min = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_min, LV_SYMBOL_DOWN " Min: 18.0C");
    lv_obj_set_style_text_color(lbl_temp_min, COLOR_TEMP_COLD, 0);
    lv_obj_set_style_text_font(lbl_temp_min, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_temp_min, LV_ALIGN_BOTTOM_RIGHT, -10, -5);
}

// ============================================================================
// Crear Panel Humedad (con gauge circular)
// ============================================================================

void createHumidityPanel(lv_obj_t *parent, int x, int y) {
    panel_humidity = createCard(parent, x, y, 325, 240);

    // Indicador + Titulo
    lv_obj_t *indicator = lv_label_create(panel_humidity);
    lv_label_set_text(indicator, "|");
    lv_obj_set_style_text_color(indicator, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_18, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *title = lv_label_create(panel_humidity);
    lv_label_set_text(title, "HUMEDAD");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align_to(title, indicator, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Icono de humedad (Weather Icons)
    lv_obj_t *hum_icon = lv_label_create(panel_humidity);
    lv_label_set_text(hum_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(hum_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(hum_icon, COLOR_CYAN, 0);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_RIGHT, -10, 30);

    // Valor GRANDE
    lbl_humidity_value = lv_label_create(panel_humidity);
    lv_label_set_text(lbl_humidity_value, "65");
    lv_obj_set_style_text_color(lbl_humidity_value, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_humidity_value, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_humidity_value, LV_ALIGN_TOP_LEFT, 10, 35);

    lv_obj_t *percent = lv_label_create(panel_humidity);
    lv_label_set_text(percent, "%");
    lv_obj_set_style_text_color(percent, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(percent, &lv_font_montserrat_24, 0);
    lv_obj_align_to(percent, lbl_humidity_value, LV_ALIGN_OUT_RIGHT_TOP, 5, 8);

    // Descripcion
    lbl_humidity_label = lv_label_create(panel_humidity);
    lv_label_set_text(lbl_humidity_label, "Humedad relativa");
    lv_obj_set_style_text_color(lbl_humidity_label, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_humidity_label, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_humidity_label, LV_ALIGN_TOP_LEFT, 10, 95);

    // Indoor (si hay BME280)
    lv_obj_t *indoor_lbl = lv_label_create(panel_humidity);
    lv_label_set_text(indoor_lbl, "Interior:");
    lv_obj_set_style_text_color(indoor_lbl, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(indoor_lbl, &lv_font_montserrat_16, 0);
    lv_obj_align(indoor_lbl, LV_ALIGN_TOP_LEFT, 10, 130);

    lbl_indoor_humidity = lv_label_create(panel_humidity);
    lv_label_set_text(lbl_indoor_humidity, "58%");
    lv_obj_set_style_text_color(lbl_indoor_humidity, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_indoor_humidity, &lv_font_montserrat_20, 0);
    lv_obj_align_to(lbl_indoor_humidity, indoor_lbl, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Max/Min
    arc_humidity = NULL;  // No usamos arco

    lv_obj_t *max_lbl = lv_label_create(panel_humidity);
    lv_label_set_text(max_lbl, LV_SYMBOL_UP " Max: 95%");
    lv_obj_set_style_text_color(max_lbl, COLOR_TEMP_HOT, 0);
    lv_obj_set_style_text_font(max_lbl, &lv_font_montserrat_18, 0);
    lv_obj_align(max_lbl, LV_ALIGN_BOTTOM_LEFT, 10, -5);

    lv_obj_t *min_lbl = lv_label_create(panel_humidity);
    lv_label_set_text(min_lbl, LV_SYMBOL_DOWN " Min: 48%");
    lv_obj_set_style_text_color(min_lbl, COLOR_TEMP_COLD, 0);
    lv_obj_set_style_text_font(min_lbl, &lv_font_montserrat_18, 0);
    lv_obj_align(min_lbl, LV_ALIGN_BOTTOM_RIGHT, -10, -5);
}

// ============================================================================
// Crear Panel Condiciones
// ============================================================================

void createConditionsPanel(lv_obj_t *parent, int x, int y, int w) {
    panel_conditions = createCard(parent, x, y, w, 185);

    // Icono condición (usando Weather Icons font)
    lbl_condition_icon = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_condition_icon, WI_DAY_SUNNY);
    lv_obj_set_style_text_font(lbl_condition_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(lbl_condition_icon, COLOR_SUN, 0);
    lv_obj_align(lbl_condition_icon, LV_ALIGN_TOP_LEFT, 10, 5);

    // Texto condición
    lbl_condition_text = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_condition_text, "DESPEJADO");
    lv_obj_set_style_text_color(lbl_condition_text, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_condition_text, &lv_font_montserrat_24, 0);
    lv_obj_align(lbl_condition_text, LV_ALIGN_TOP_LEFT, 80, 15);

    // Viento
    lbl_wind = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_wind, "V: 12 km/h NE");
    lv_obj_set_style_text_color(lbl_wind, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_wind, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_wind, LV_ALIGN_TOP_LEFT, 10, 65);

    // Rafaga
    lbl_wind_gust = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_wind_gust, "Rafaga: 18 km/h");
    lv_obj_set_style_text_color(lbl_wind_gust, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_wind_gust, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_wind_gust, LV_ALIGN_TOP_LEFT, 150, 67);

    // UV
    lbl_uv = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_uv, "* UV: 3 (Moderado)");
    lv_obj_set_style_text_color(lbl_uv, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_uv, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_uv, LV_ALIGN_TOP_LEFT, 10, 95);

    // Punto de rocío
    lbl_dewpoint = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_dewpoint, "T: Rocio: 14°C");
    lv_obj_set_style_text_color(lbl_dewpoint, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_dewpoint, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_dewpoint, LV_ALIGN_TOP_LEFT, 200, 97);

    // Lluvia
    lbl_rain = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_rain, "Lluvia: Lluvia: 0.0 mm/h  ·  Hoy: 2.5 mm");
    lv_obj_set_style_text_color(lbl_rain, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_rain, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_rain, LV_ALIGN_TOP_LEFT, 10, 125);
}

// ============================================================================
// Crear Panel Pronóstico
// ============================================================================

void createForecastPanel(lv_obj_t *parent, int x, int y) {
    panel_forecast = createCard(parent, x, y, 350, 95);

    // Indicador + Titulo
    lv_obj_t *indicator = lv_label_create(panel_forecast);
    lv_label_set_text(indicator, "|");
    lv_obj_set_style_text_color(indicator, COLOR_ACCENT, 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_14, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *title = lv_label_create(panel_forecast);
    lv_label_set_text(title, "PRONOSTICO");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_align_to(title, indicator, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    // 5 tarjetas de dias
    const char* days[] = {"Hoy", "Mie", "Jue", "Vie", "Sab"};
    const char* temps[] = {"23", "22", "23", "22", "22"};
    const char* mins[] = {"13", "14", "12", "14", "13"};
    const char* rain[] = {"64%", "85%", "91%", "100%", "80%"};

    int card_w = 62;
    int card_h = 68;
    int start_x = 5;
    int start_y = 18;

    for (int i = 0; i < 5; i++) {
        forecast_cards[i] = lv_obj_create(panel_forecast);
        lv_obj_set_size(forecast_cards[i], card_w, card_h);
        lv_obj_set_pos(forecast_cards[i], start_x + i * (card_w + 4), start_y);
        lv_obj_set_style_bg_color(forecast_cards[i], COLOR_BG, 0);
        lv_obj_set_style_radius(forecast_cards[i], 8, 0);
        lv_obj_set_style_border_width(forecast_cards[i], 1, 0);
        lv_obj_set_style_border_color(forecast_cards[i], COLOR_BORDER, 0);
        lv_obj_set_style_pad_all(forecast_cards[i], 4, 0);
        lv_obj_clear_flag(forecast_cards[i], LV_OBJ_FLAG_SCROLLABLE);

        // Dia
        lbl_fc_day[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_day[i], days[i]);
        lv_obj_set_style_text_color(lbl_fc_day[i], COLOR_TEXT_MUTED, 0);
        lv_obj_set_style_text_font(lbl_fc_day[i], &lv_font_montserrat_12, 0);
        lv_obj_align(lbl_fc_day[i], LV_ALIGN_TOP_MID, 0, 0);

        // Temperatura (max)
        lbl_fc_temp[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_temp[i], temps[i]);
        lv_obj_set_style_text_color(lbl_fc_temp[i], COLOR_TEMP_HOT, 0);
        lv_obj_set_style_text_font(lbl_fc_temp[i], &lv_font_montserrat_16, 0);
        lv_obj_align(lbl_fc_temp[i], LV_ALIGN_CENTER, 0, -3);

        // Min
        lbl_fc_icon[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_icon[i], mins[i]);
        lv_obj_set_style_text_color(lbl_fc_icon[i], COLOR_TEXT_MUTED, 0);
        lv_obj_set_style_text_font(lbl_fc_icon[i], &lv_font_montserrat_12, 0);
        lv_obj_align_to(lbl_fc_icon[i], lbl_fc_temp[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        // Prob lluvia
        lbl_fc_rain[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_rain[i], rain[i]);
        lv_obj_set_style_text_color(lbl_fc_rain[i], COLOR_RAIN, 0);
        lv_obj_set_style_text_font(lbl_fc_rain[i], &lv_font_montserrat_12, 0);
        lv_obj_align(lbl_fc_rain[i], LV_ALIGN_BOTTOM_MID, 0, 0);
    }
}

// ============================================================================
// Crear Panel Interior (BME280 local o consola)
// ============================================================================

void createIndoorPanel(lv_obj_t *parent, int x, int y, int w) {
    panel_indoor = createCard(parent, x, y, w, 130);

    // Título
    lbl_indoor_title = lv_label_create(panel_indoor);
    lv_label_set_text(lbl_indoor_title, "* INTERIOR");
    lv_obj_set_style_text_color(lbl_indoor_title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_indoor_title, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_indoor_title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Temperatura interior
    lbl_indoor_temp = lv_label_create(panel_indoor);
    lv_label_set_text(lbl_indoor_temp, "22.3°C");
    lv_obj_set_style_text_color(lbl_indoor_temp, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_indoor_temp, &lv_font_montserrat_28, 0);
    lv_obj_align(lbl_indoor_temp, LV_ALIGN_TOP_LEFT, 10, 30);

    // Humedad interior
    lbl_indoor_humidity = lv_label_create(panel_indoor);
    lv_label_set_text(lbl_indoor_humidity, "H: 58%");
    lv_obj_set_style_text_color(lbl_indoor_humidity, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_indoor_humidity, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_indoor_humidity, LV_ALIGN_TOP_LEFT, 130, 35);

    // Presion interior
    lbl_indoor_pressure = lv_label_create(panel_indoor);
    lv_label_set_text(lbl_indoor_pressure, "P: 1018 hPa");
    lv_obj_set_style_text_color(lbl_indoor_pressure, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_indoor_pressure, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_indoor_humidity, LV_ALIGN_TOP_LEFT, 230, 35);

    // Diferencia vs exterior
    lbl_indoor_diff = lv_label_create(panel_indoor);
    lv_label_set_text(lbl_indoor_diff, "(vs exterior: +0.8°C)");
    lv_obj_set_style_text_color(lbl_indoor_diff, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_indoor_diff, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_indoor_diff, LV_ALIGN_BOTTOM_LEFT, 10, -5);
}

// ============================================================================
// Mini-cards para fila superior (estilo servidor)
// ============================================================================

static lv_obj_t *mini_humidity = nullptr;
static lv_obj_t *mini_uv = nullptr;
static lv_obj_t *mini_pressure = nullptr;
static lv_obj_t *lbl_mini_hum_val = nullptr;
static lv_obj_t *lbl_mini_uv_val = nullptr;
static lv_obj_t *lbl_mini_pres_val = nullptr;

lv_obj_t* createMiniCard(lv_obj_t *parent, int x, int y, int w, int h,
                         const char* title, lv_color_t valueColor, lv_obj_t **valueLabel) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_set_style_bg_color(card, COLOR_CARD, 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, COLOR_BORDER, 0);
    lv_obj_set_style_pad_all(card, 8, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    // Titulo
    lv_obj_t *lbl_title = lv_label_create(card);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_color(lbl_title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Valor
    *valueLabel = lv_label_create(card);
    lv_label_set_text(*valueLabel, "--");
    lv_obj_set_style_text_color(*valueLabel, valueColor, 0);
    lv_obj_set_style_text_font(*valueLabel, &lv_font_montserrat_24, 0);
    lv_obj_align(*valueLabel, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    return card;
}

// ============================================================================
// Panel Viento estilo servidor (con brujula simplificada)
// ============================================================================

static lv_obj_t *panel_wind_new = nullptr;
static lv_obj_t *lbl_wind_speed = nullptr;
static lv_obj_t *lbl_wind_dir_icon = nullptr;
static lv_obj_t *lbl_wind_direction = nullptr;
static lv_obj_t *lbl_wind_gust_new = nullptr;
static lv_obj_t *lbl_wind_beaufort = nullptr;

void createWindPanel(lv_obj_t *parent, int x, int y, int w, int h) {
    panel_wind_new = createCard(parent, x, y, w, h);

    // Indicador + Titulo
    lv_obj_t *indicator = lv_label_create(panel_wind_new);
    lv_label_set_text(indicator, "|");
    lv_obj_set_style_text_color(indicator, COLOR_WIND, 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_18, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *title = lv_label_create(panel_wind_new);
    lv_label_set_text(title, "VIENTO");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align_to(title, indicator, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Icono de viento (Weather Icons)
    lv_obj_t *wind_icon = lv_label_create(panel_wind_new);
    lv_label_set_text(wind_icon, WI_STRONG_WIND);
    lv_obj_set_style_text_font(wind_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(wind_icon, COLOR_WIND, 0);
    lv_obj_align(wind_icon, LV_ALIGN_TOP_MID, 0, 2);

    // Velocidad GRANDE
    lbl_wind_speed = lv_label_create(panel_wind_new);
    lv_label_set_text(lbl_wind_speed, "14.0");
    lv_obj_set_style_text_color(lbl_wind_speed, COLOR_WIND, 0);
    lv_obj_set_style_text_font(lbl_wind_speed, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_wind_speed, LV_ALIGN_TOP_LEFT, 10, 35);

    lv_obj_t *unit = lv_label_create(panel_wind_new);
    lv_label_set_text(unit, "km/h");
    lv_obj_set_style_text_color(unit, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(unit, &lv_font_montserrat_20, 0);
    lv_obj_align_to(unit, lbl_wind_speed, LV_ALIGN_OUT_RIGHT_BOTTOM, 8, -5);

    // Icono de direccion (Weather Icons)
    lbl_wind_dir_icon = lv_label_create(panel_wind_new);
    lv_label_set_text(lbl_wind_dir_icon, WI_DIR_N);
    lv_obj_set_style_text_font(lbl_wind_dir_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(lbl_wind_dir_icon, COLOR_WIND, 0);
    lv_obj_align(lbl_wind_dir_icon, LV_ALIGN_TOP_RIGHT, -15, 35);

    // Direccion texto
    lbl_wind_direction = lv_label_create(panel_wind_new);
    lv_label_set_text(lbl_wind_direction, "NO 303");
    lv_obj_set_style_text_color(lbl_wind_direction, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_wind_direction, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_wind_direction, LV_ALIGN_TOP_RIGHT, -10, 75);

    // Rafaga
    lv_obj_t *gust_label = lv_label_create(panel_wind_new);
    lv_label_set_text(gust_label, "Rafaga:");
    lv_obj_set_style_text_color(gust_label, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(gust_label, &lv_font_montserrat_16, 0);
    lv_obj_align(gust_label, LV_ALIGN_TOP_LEFT, 10, 130);

    lbl_wind_gust_new = lv_label_create(panel_wind_new);
    lv_label_set_text(lbl_wind_gust_new, "18.5 km/h");
    lv_obj_set_style_text_color(lbl_wind_gust_new, COLOR_TEMP_HOT, 0);
    lv_obj_set_style_text_font(lbl_wind_gust_new, &lv_font_montserrat_20, 0);
    lv_obj_align_to(lbl_wind_gust_new, gust_label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Escala Beaufort
    lbl_wind_beaufort = lv_label_create(panel_wind_new);
    lv_label_set_text(lbl_wind_beaufort, "Escala Beaufort: 3 - Brisa debil");
    lv_obj_set_style_text_color(lbl_wind_beaufort, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_wind_beaufort, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_wind_beaufort, LV_ALIGN_BOTTOM_LEFT, 10, -5);
}

// ============================================================================
// Panel Precipitacion estilo servidor
// ============================================================================

static lv_obj_t *panel_rain_new = nullptr;
static lv_obj_t *lbl_rain_rate = nullptr;
static lv_obj_t *lbl_rain_today = nullptr;
static lv_obj_t *lbl_rain_month = nullptr;
static lv_obj_t *lbl_rain_year = nullptr;

void createRainPanel(lv_obj_t *parent, int x, int y, int w, int h) {
    panel_rain_new = createCard(parent, x, y, w, h);

    // Indicador + Titulo
    lv_obj_t *indicator = lv_label_create(panel_rain_new);
    lv_label_set_text(indicator, "|");
    lv_obj_set_style_text_color(indicator, COLOR_RAIN, 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_18, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *title = lv_label_create(panel_rain_new);
    lv_label_set_text(title, "PRECIPITACION");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align_to(title, indicator, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Icono de lluvia (Weather Icons)
    lv_obj_t *rain_icon = lv_label_create(panel_rain_new);
    lv_label_set_text(rain_icon, WI_UMBRELLA);
    lv_obj_set_style_text_font(rain_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(rain_icon, COLOR_RAIN, 0);
    lv_obj_align(rain_icon, LV_ALIGN_TOP_RIGHT, -10, 30);

    // Tasa actual GRANDE
    lbl_rain_rate = lv_label_create(panel_rain_new);
    lv_label_set_text(lbl_rain_rate, "0.0");
    lv_obj_set_style_text_color(lbl_rain_rate, COLOR_RAIN, 0);
    lv_obj_set_style_text_font(lbl_rain_rate, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_rain_rate, LV_ALIGN_TOP_LEFT, 10, 35);

    lv_obj_t *unit = lv_label_create(panel_rain_new);
    lv_label_set_text(unit, "mm/h");
    lv_obj_set_style_text_color(unit, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(unit, &lv_font_montserrat_20, 0);
    lv_obj_align_to(unit, lbl_rain_rate, LV_ALIGN_OUT_RIGHT_BOTTOM, 8, -5);

    // Acumulados
    lv_obj_t *today_lbl = lv_label_create(panel_rain_new);
    lv_label_set_text(today_lbl, "Hoy:");
    lv_obj_set_style_text_color(today_lbl, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(today_lbl, &lv_font_montserrat_16, 0);
    lv_obj_align(today_lbl, LV_ALIGN_TOP_LEFT, 10, 95);

    lbl_rain_today = lv_label_create(panel_rain_new);
    lv_label_set_text(lbl_rain_today, "2.5 mm");
    lv_obj_set_style_text_color(lbl_rain_today, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_rain_today, &lv_font_montserrat_20, 0);
    lv_obj_align_to(lbl_rain_today, today_lbl, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Semana
    lv_obj_t *week_lbl = lv_label_create(panel_rain_new);
    lv_label_set_text(week_lbl, "Semana:");
    lv_obj_set_style_text_color(week_lbl, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(week_lbl, &lv_font_montserrat_16, 0);
    lv_obj_align(week_lbl, LV_ALIGN_TOP_LEFT, 10, 125);

    lbl_rain_month = lv_label_create(panel_rain_new);
    lv_label_set_text(lbl_rain_month, "15.2 mm");
    lv_obj_set_style_text_color(lbl_rain_month, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_rain_month, &lv_font_montserrat_20, 0);
    lv_obj_align_to(lbl_rain_month, week_lbl, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Mes/Ano (abajo)
    lbl_rain_year = lv_label_create(panel_rain_new);
    lv_label_set_text(lbl_rain_year, "Mes: 45 mm  |  Ano: 320 mm");
    lv_obj_set_style_text_color(lbl_rain_year, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_rain_year, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_rain_year, LV_ALIGN_BOTTOM_LEFT, 10, -5);
}

// ============================================================================
// Panel Sol y Luna estilo servidor
// ============================================================================

static lv_obj_t *panel_sun_moon = nullptr;
static lv_obj_t *lbl_sunrise_val = nullptr;
static lv_obj_t *lbl_sunset_val = nullptr;
static lv_obj_t *lbl_moon_icon = nullptr;
static lv_obj_t *lbl_moon_val = nullptr;
static lv_obj_t *lbl_daylight = nullptr;

void createSunMoonPanel(lv_obj_t *parent, int x, int y, int w, int h) {
    panel_sun_moon = createCard(parent, x, y, w, h);

    // Indicador + Titulo
    lv_obj_t *indicator = lv_label_create(panel_sun_moon);
    lv_label_set_text(indicator, "|");
    lv_obj_set_style_text_color(indicator, COLOR_SUN, 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_18, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *title = lv_label_create(panel_sun_moon);
    lv_label_set_text(title, "SOL Y LUNA");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align_to(title, indicator, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Amanecer - con icono
    lv_obj_t *sunrise_icon = lv_label_create(panel_sun_moon);
    lv_label_set_text(sunrise_icon, WI_SUNRISE);
    lv_obj_set_style_text_font(sunrise_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(sunrise_icon, COLOR_SUN, 0);
    lv_obj_align(sunrise_icon, LV_ALIGN_TOP_LEFT, 10, 30);

    lbl_sunrise_val = lv_label_create(panel_sun_moon);
    lv_label_set_text(lbl_sunrise_val, "06:06");
    lv_obj_set_style_text_color(lbl_sunrise_val, COLOR_SUN, 0);
    lv_obj_set_style_text_font(lbl_sunrise_val, &lv_font_montserrat_24, 0);
    lv_obj_align_to(lbl_sunrise_val, sunrise_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Atardecer - con icono
    lv_obj_t *sunset_icon = lv_label_create(panel_sun_moon);
    lv_label_set_text(sunset_icon, WI_SUNSET);
    lv_obj_set_style_text_font(sunset_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(sunset_icon, COLOR_TEMP_HOT, 0);
    lv_obj_align(sunset_icon, LV_ALIGN_TOP_LEFT, 10, 65);

    lbl_sunset_val = lv_label_create(panel_sun_moon);
    lv_label_set_text(lbl_sunset_val, "19:18");
    lv_obj_set_style_text_color(lbl_sunset_val, COLOR_TEMP_HOT, 0);
    lv_obj_set_style_text_font(lbl_sunset_val, &lv_font_montserrat_24, 0);
    lv_obj_align_to(lbl_sunset_val, sunset_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Luna - Icono (Weather Icons)
    lbl_moon_icon = lv_label_create(panel_sun_moon);
    lv_label_set_text(lbl_moon_icon, WI_MOON_NEW);
    lv_obj_set_style_text_font(lbl_moon_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(lbl_moon_icon, COLOR_TEXT_SECONDARY, 0);
    lv_obj_align(lbl_moon_icon, LV_ALIGN_TOP_LEFT, 10, 100);

    lbl_moon_val = lv_label_create(panel_sun_moon);
    lv_label_set_text(lbl_moon_val, "Nueva (5%)");
    lv_obj_set_style_text_color(lbl_moon_val, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_moon_val, &lv_font_montserrat_18, 0);
    lv_obj_align_to(lbl_moon_val, lbl_moon_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Horas de luz
    lv_obj_t *daylight_lbl = lv_label_create(panel_sun_moon);
    lv_label_set_text(daylight_lbl, "Horas de luz:");
    lv_obj_set_style_text_color(daylight_lbl, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(daylight_lbl, &lv_font_montserrat_16, 0);
    lv_obj_align(daylight_lbl, LV_ALIGN_TOP_LEFT, 10, 140);

    lbl_daylight = lv_label_create(panel_sun_moon);
    lv_label_set_text(lbl_daylight, "13 h 12 min");
    lv_obj_set_style_text_color(lbl_daylight, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_daylight, &lv_font_montserrat_20, 0);
    lv_obj_align_to(lbl_daylight, daylight_lbl, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Indice UV (abajo)
    lv_obj_t *uv_lbl = lv_label_create(panel_sun_moon);
    lv_label_set_text(uv_lbl, "Indice UV: 3 (Moderado)");
    lv_obj_set_style_text_color(uv_lbl, COLOR_UV, 0);
    lv_obj_set_style_text_font(uv_lbl, &lv_font_montserrat_16, 0);
    lv_obj_align(uv_lbl, LV_ALIGN_BOTTOM_LEFT, 10, -5);
}

// ============================================================================
// NUEVO LAYOUT: Panel Principal + 3 Cards de ubicacion
// ============================================================================

// Variables para el nuevo layout
static lv_obj_t *panel_main = nullptr;
static lv_obj_t *lbl_main_temp = nullptr;
static lv_obj_t *lbl_main_condition_icon = nullptr;
static lv_obj_t *lbl_main_condition = nullptr;
static lv_obj_t *lbl_main_feels = nullptr;
static lv_obj_t *lbl_main_maxmin = nullptr;
static lv_obj_t *lbl_main_humidity = nullptr;
static lv_obj_t *lbl_main_pressure = nullptr;
static lv_obj_t *lbl_main_pressure_trend = nullptr;
static lv_obj_t *lbl_main_wind = nullptr;
static lv_obj_t *lbl_main_wind_dir = nullptr;
static lv_obj_t *lbl_main_wind_icon = nullptr;
static lv_obj_t *lbl_main_gust = nullptr;
static lv_obj_t *lbl_main_server = nullptr;

static lv_obj_t *card_local = nullptr;
static lv_obj_t *lbl_local_temp = nullptr;
static lv_obj_t *lbl_local_maxmin = nullptr;
static lv_obj_t *lbl_local_humidity = nullptr;
static lv_obj_t *lbl_local_battery = nullptr;

static lv_obj_t *card_jardin = nullptr;
static lv_obj_t *lbl_jardin_temp = nullptr;
static lv_obj_t *lbl_jardin_maxmin = nullptr;
static lv_obj_t *lbl_jardin_humidity = nullptr;
static lv_obj_t *lbl_jardin_battery = nullptr;

static lv_obj_t *card_remoto = nullptr;
static lv_obj_t *lbl_remoto_temp = nullptr;
static lv_obj_t *lbl_remoto_maxmin = nullptr;
static lv_obj_t *lbl_remoto_humidity = nullptr;
static lv_obj_t *lbl_remoto_pressure = nullptr;

/**
 * Panel Principal - Estacion Ecowitt con toda la info
 */
void createMainPanel(lv_obj_t *parent, int x, int y, int w, int h) {
    panel_main = createCard(parent, x, y, w, h);

    // --- Columna izquierda: Temperatura y condicion ---

    // Icono de condicion grande
    lbl_main_condition_icon = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_condition_icon, WI_DAY_SUNNY);
    lv_obj_set_style_text_font(lbl_main_condition_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(lbl_main_condition_icon, COLOR_SUN, 0);
    lv_obj_align(lbl_main_condition_icon, LV_ALIGN_TOP_LEFT, 15, 15);

    // Temperatura grande
    lbl_main_temp = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_temp, "24.5°");
    lv_obj_set_style_text_color(lbl_main_temp, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_main_temp, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_main_temp, LV_ALIGN_TOP_LEFT, 80, 10);

    // Condicion texto
    lbl_main_condition = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_condition, "Despejado");
    lv_obj_set_style_text_color(lbl_main_condition, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_main_condition, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_main_condition, LV_ALIGN_TOP_LEFT, 15, 75);

    // Sensacion termica
    lbl_main_feels = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_feels, "Sensacion 23.5°");
    lv_obj_set_style_text_color(lbl_main_feels, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_main_feels, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_main_feels, LV_ALIGN_TOP_LEFT, 15, 100);

    // Max / Min
    lbl_main_maxmin = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_maxmin, LV_SYMBOL_UP " 28°  " LV_SYMBOL_DOWN " 18°");
    lv_obj_set_style_text_color(lbl_main_maxmin, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_main_maxmin, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_main_maxmin, LV_ALIGN_TOP_LEFT, 15, 125);

    // --- Columna central: Humedad y Presion ---

    // Titulo humedad
    lv_obj_t *hum_title = lv_label_create(panel_main);
    lv_label_set_text(hum_title, "Humedad");
    lv_obj_set_style_text_color(hum_title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(hum_title, &lv_font_montserrat_14, 0);
    lv_obj_align(hum_title, LV_ALIGN_TOP_MID, -100, 10);

    // Icono + valor humedad
    lv_obj_t *hum_icon = lv_label_create(panel_main);
    lv_label_set_text(hum_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(hum_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(hum_icon, COLOR_CYAN, 0);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_MID, -120, 30);

    lbl_main_humidity = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_humidity, "65%");
    lv_obj_set_style_text_color(lbl_main_humidity, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(lbl_main_humidity, &lv_font_montserrat_36, 0);
    lv_obj_align(lbl_main_humidity, LV_ALIGN_TOP_MID, -60, 30);

    // Titulo presion
    lv_obj_t *pres_title = lv_label_create(panel_main);
    lv_label_set_text(pres_title, "Presion");
    lv_obj_set_style_text_color(pres_title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(pres_title, &lv_font_montserrat_14, 0);
    lv_obj_align(pres_title, LV_ALIGN_TOP_MID, -100, 80);

    // Icono + valor presion
    lv_obj_t *pres_icon = lv_label_create(panel_main);
    lv_label_set_text(pres_icon, WI_BAROMETER);
    lv_obj_set_style_text_font(pres_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(pres_icon, COLOR_VIOLET, 0);
    lv_obj_align(pres_icon, LV_ALIGN_TOP_MID, -120, 100);

    lbl_main_pressure = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_pressure, "1012");
    lv_obj_set_style_text_color(lbl_main_pressure, COLOR_VIOLET, 0);
    lv_obj_set_style_text_font(lbl_main_pressure, &lv_font_montserrat_28, 0);
    lv_obj_align(lbl_main_pressure, LV_ALIGN_TOP_MID, -55, 100);

    lbl_main_pressure_trend = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_pressure_trend, LV_SYMBOL_UP);
    lv_obj_set_style_text_color(lbl_main_pressure_trend, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_main_pressure_trend, &lv_font_montserrat_18, 0);
    lv_obj_align_to(lbl_main_pressure_trend, lbl_main_pressure, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    // --- Columna derecha: Viento ---

    // Titulo viento
    lv_obj_t *wind_title = lv_label_create(panel_main);
    lv_label_set_text(wind_title, "Viento");
    lv_obj_set_style_text_color(wind_title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(wind_title, &lv_font_montserrat_14, 0);
    lv_obj_align(wind_title, LV_ALIGN_TOP_RIGHT, -150, 10);

    // Icono viento
    lv_obj_t *wind_icon = lv_label_create(panel_main);
    lv_label_set_text(wind_icon, WI_STRONG_WIND);
    lv_obj_set_style_text_font(wind_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(wind_icon, COLOR_WIND, 0);
    lv_obj_align(wind_icon, LV_ALIGN_TOP_RIGHT, -180, 30);

    // Velocidad viento
    lbl_main_wind = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_wind, "12 km/h");
    lv_obj_set_style_text_color(lbl_main_wind, COLOR_WIND, 0);
    lv_obj_set_style_text_font(lbl_main_wind, &lv_font_montserrat_28, 0);
    lv_obj_align(lbl_main_wind, LV_ALIGN_TOP_RIGHT, -50, 30);

    // Direccion con icono
    lbl_main_wind_icon = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_wind_icon, WI_DIR_N);
    lv_obj_set_style_text_font(lbl_main_wind_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(lbl_main_wind_icon, COLOR_WIND, 0);
    lv_obj_align(lbl_main_wind_icon, LV_ALIGN_TOP_RIGHT, -180, 70);

    lbl_main_wind_dir = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_wind_dir, "NO 303°");
    lv_obj_set_style_text_color(lbl_main_wind_dir, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_main_wind_dir, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_main_wind_dir, LV_ALIGN_TOP_RIGHT, -80, 75);

    // Rafagas
    lbl_main_gust = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_gust, "Rafagas: 18 km/h");
    lv_obj_set_style_text_color(lbl_main_gust, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_main_gust, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_main_gust, LV_ALIGN_TOP_RIGHT, -50, 110);

    // --- Barra inferior: Server status ---
    lbl_main_server = lv_label_create(panel_main);
    lv_label_set_text(lbl_main_server, "Servidor: Online  |  Actualizado: 12:34:56");
    lv_obj_set_style_text_color(lbl_main_server, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_main_server, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_main_server, LV_ALIGN_BOTTOM_LEFT, 15, -10);
}

/**
 * Card de ubicacion generica (Local, Jardin, Remoto)
 */
lv_obj_t* createLocationCard(lv_obj_t *parent, int x, int y, int w, int h,
                             const char* title, lv_color_t accent,
                             lv_obj_t** temp_lbl, lv_obj_t** maxmin_lbl,
                             lv_obj_t** humidity_lbl, lv_obj_t** extra_lbl,
                             const char* extra_label_text) {
    lv_obj_t *card = createCard(parent, x, y, w, h);

    // Indicador de color + titulo
    lv_obj_t *indicator = lv_label_create(card);
    lv_label_set_text(indicator, "|");
    lv_obj_set_style_text_color(indicator, accent, 0);
    lv_obj_set_style_text_font(indicator, &lv_font_montserrat_18, 0);
    lv_obj_align(indicator, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *title_lbl = lv_label_create(card);
    lv_label_set_text(title_lbl, title);
    lv_obj_set_style_text_color(title_lbl, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title_lbl, &lv_font_montserrat_14, 0);
    lv_obj_align_to(title_lbl, indicator, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // Icono termometro
    lv_obj_t *thermo = lv_label_create(card);
    lv_label_set_text(thermo, WI_THERMOMETER);
    lv_obj_set_style_text_font(thermo, &weather_icons_32, 0);
    lv_obj_set_style_text_color(thermo, accent, 0);
    lv_obj_align(thermo, LV_ALIGN_TOP_LEFT, 5, 30);

    // Temperatura grande
    *temp_lbl = lv_label_create(card);
    lv_label_set_text(*temp_lbl, "22.5°");
    lv_obj_set_style_text_color(*temp_lbl, accent, 0);
    lv_obj_set_style_text_font(*temp_lbl, &lv_font_montserrat_36, 0);
    lv_obj_align(*temp_lbl, LV_ALIGN_TOP_LEFT, 50, 25);

    // Max/Min
    *maxmin_lbl = lv_label_create(card);
    lv_label_set_text(*maxmin_lbl, LV_SYMBOL_UP " 25°  " LV_SYMBOL_DOWN " 20°");
    lv_obj_set_style_text_color(*maxmin_lbl, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(*maxmin_lbl, &lv_font_montserrat_14, 0);
    lv_obj_align(*maxmin_lbl, LV_ALIGN_TOP_LEFT, 10, 75);

    // Humedad
    lv_obj_t *hum_icon = lv_label_create(card);
    lv_label_set_text(hum_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(hum_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(hum_icon, COLOR_CYAN, 0);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_LEFT, 5, 100);

    *humidity_lbl = lv_label_create(card);
    lv_label_set_text(*humidity_lbl, "58%");
    lv_obj_set_style_text_color(*humidity_lbl, COLOR_CYAN, 0);
    lv_obj_set_style_text_font(*humidity_lbl, &lv_font_montserrat_24, 0);
    lv_obj_align(*humidity_lbl, LV_ALIGN_TOP_LEFT, 50, 105);

    // Campo extra (bateria o presion)
    if (extra_lbl && extra_label_text) {
        lv_obj_t *extra_title = lv_label_create(card);
        lv_label_set_text(extra_title, extra_label_text);
        lv_obj_set_style_text_color(extra_title, COLOR_TEXT_MUTED, 0);
        lv_obj_set_style_text_font(extra_title, &lv_font_montserrat_12, 0);
        lv_obj_align(extra_title, LV_ALIGN_TOP_RIGHT, -10, 100);

        *extra_lbl = lv_label_create(card);
        lv_label_set_text(*extra_lbl, "--");
        lv_obj_set_style_text_color(*extra_lbl, COLOR_TEXT_SECONDARY, 0);
        lv_obj_set_style_text_font(*extra_lbl, &lv_font_montserrat_18, 0);
        lv_obj_align(*extra_lbl, LV_ALIGN_TOP_RIGHT, -10, 118);
    }

    return card;
}

// ============================================================================
// Crear Dashboard completo - NUEVO LAYOUT (1 Principal + 3 Ubicaciones)
// ============================================================================

void createDashboard() {
    initDashboardStyles();

    // Crear pantalla base
    scr_dashboard = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_dashboard, COLOR_BG, 0);
    lv_obj_clear_flag(scr_dashboard, LV_OBJ_FLAG_SCROLLABLE);

    // Header
    createHeader(scr_dashboard);

    // === NUEVO LAYOUT: 1 Principal + 3 Ubicaciones ===
    // Dimensiones
    int gap = 12;
    int y1 = HEADER_HEIGHT + gap;
    int main_h = 160;           // Panel principal mas compacto
    int y2 = y1 + main_h + gap; // Segunda fila
    int card_h = 150;           // Cards de ubicacion
    int card_w = (SCREEN_WIDTH - gap * 4) / 3;  // ~327px cada una

    // Panel Principal (ancho completo)
    createMainPanel(scr_dashboard, gap, y1, SCREEN_WIDTH - gap * 2, main_h);

    // === FILA 2: Cards de ubicacion ===

    // Card LOCAL (BME280) - tambien lleva bateria
    card_local = createLocationCard(scr_dashboard,
        gap, y2, card_w, card_h,
        "LOCAL (Interior)", COLOR_GREEN,
        &lbl_local_temp, &lbl_local_maxmin, &lbl_local_humidity,
        &lbl_local_battery, "Bateria:");

    // Card JARDIN (HN31)
    card_jardin = createLocationCard(scr_dashboard,
        gap + card_w + gap, y2, card_w, card_h,
        "JARDIN (HN31)", COLOR_WIND,
        &lbl_jardin_temp, &lbl_jardin_maxmin, &lbl_jardin_humidity,
        &lbl_jardin_battery, "Bateria:");

    // Card REMOTO (GW110)
    card_remoto = createLocationCard(scr_dashboard,
        gap + (card_w + gap) * 2, y2, card_w, card_h,
        "REMOTO (GW110)", COLOR_VIOLET,
        &lbl_remoto_temp, &lbl_remoto_maxmin, &lbl_remoto_humidity,
        &lbl_remoto_pressure, "Presion:");

    // === TAP HANDLERS para navegacion a pantallas de detalle ===
    // Definiciones de pantallas (deben coincidir con ui_navigation.h)
    #define NAV_SCREEN_DETAIL_PRINCIPAL 1
    #define NAV_SCREEN_DETAIL_LOCAL     2
    #define NAV_SCREEN_DETAIL_JARDIN    3
    #define NAV_SCREEN_DETAIL_REMOTO    4

    // Panel Principal -> Detalle Principal
    if (panel_main) {
        lv_obj_add_flag(panel_main, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(panel_main, [](lv_event_t *e) {
            extern void navigateToScreenById(int);
            navigateToScreenById(NAV_SCREEN_DETAIL_PRINCIPAL);
        }, LV_EVENT_CLICKED, NULL);
    }

    // Card Local -> Detalle Local
    if (card_local) {
        lv_obj_add_flag(card_local, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(card_local, [](lv_event_t *e) {
            extern void navigateToScreenById(int);
            navigateToScreenById(NAV_SCREEN_DETAIL_LOCAL);
        }, LV_EVENT_CLICKED, NULL);
    }

    // Card Jardin -> Detalle Jardin
    if (card_jardin) {
        lv_obj_add_flag(card_jardin, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(card_jardin, [](lv_event_t *e) {
            extern void navigateToScreenById(int);
            navigateToScreenById(NAV_SCREEN_DETAIL_JARDIN);
        }, LV_EVENT_CLICKED, NULL);
    }

    // Card Remoto -> Detalle Remoto
    if (card_remoto) {
        lv_obj_add_flag(card_remoto, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(card_remoto, [](lv_event_t *e) {
            extern void navigateToScreenById(int);
            navigateToScreenById(NAV_SCREEN_DETAIL_REMOTO);
        }, LV_EVENT_CLICKED, NULL);
    }

    // Cargar pantalla
    lv_scr_load(scr_dashboard);

    Serial.println("[UI] Dashboard creado (nuevo layout 4 cards)");
}

// ============================================================================
// Actualizar datos en el dashboard
// ============================================================================

void updateDashboardTime() {
    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);

    // Hora en header (con segundos)
    char time_full[10];
    strftime(time_full, sizeof(time_full), "%H:%M:%S", &timeinfo);
    if (lbl_header_time) lv_label_set_text(lbl_header_time, time_full);

    // Fecha en header (estilo servidor)
    char date_str[30];
    const char* dias[] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
    const char* meses[] = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
                           "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};
    snprintf(date_str, sizeof(date_str), "%s %d de %s",
             dias[timeinfo.tm_wday], timeinfo.tm_mday, meses[timeinfo.tm_mon]);
    if (lbl_status_text) lv_label_set_text(lbl_status_text, date_str);

    // Hora en panel reloj
    char time_str[10];
    strftime(time_str, sizeof(time_str), "%H:%M", &timeinfo);
    if (lbl_clock_time) lv_label_set_text(lbl_clock_time, time_str);

    // Fecha corta en panel reloj
    char date_short[15];
    const char* dias_corto[] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
    const char* meses_corto[] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun",
                                  "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
    snprintf(date_short, sizeof(date_short), "%s %d %s",
             dias_corto[timeinfo.tm_wday], timeinfo.tm_mday, meses_corto[timeinfo.tm_mon]);
    if (lbl_clock_date) lv_label_set_text(lbl_clock_date, date_short);
}

void updateDashboardWeather() {
    extern WeatherData g_weather;
    extern CompareData g_compare;
    extern AlertData g_alerts;
    extern AlmanacData g_almanac;
    extern SystemStatus g_status;
    extern LocalSensorData g_local;

    char buf[64];

    // === Status ===
    if (lbl_status_icon) {
        lv_label_set_text(lbl_status_icon, g_status.api_ok ? "●" : "●");
        lv_obj_set_style_text_color(lbl_status_icon,
            g_status.api_ok ? COLOR_GREEN : COLOR_RED, 0);
    }
    if (lbl_status_text) {
        lv_label_set_text(lbl_status_text, g_status.api_ok ? "Online" : "Offline");
    }
    // WiFi ya se actualiza con icono y color en la funcion

    // === Derivar condicion del clima ===
    // Determinar si es de dia
    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);
    int current_hour = timeinfo.tm_hour;
    int sunrise_hour = 6, sunset_hour = 19;  // Default
    if (g_almanac.valid) {
        sscanf(g_almanac.sunrise, "%d:", &sunrise_hour);
        sscanf(g_almanac.sunset, "%d:", &sunset_hour);
        if (sunset_hour < 12) sunset_hour += 12;  // PM adjustment
    }
    bool is_day = isDaytime(current_hour, sunrise_hour, sunset_hour);

    // Derivar condicion de datos de sensores
    float pressure_diff = g_compare.valid ? g_compare.pressure_diff : 0;
    const char* condition = deriveWeatherCondition(
        g_weather.rain_rate, g_weather.wind_speed,
        g_weather.humidity_outdoor, g_weather.uv, pressure_diff);

    // Actualizar icono de condicion
    if (lbl_condition_icon) {
        const char* icon = getWeatherIcon(condition, is_day);
        lv_label_set_text(lbl_condition_icon, icon);

        // Color segun condicion
        lv_color_t icon_color = COLOR_SUN;
        if (strstr(condition, "rain") || strstr(condition, "drizzle")) {
            icon_color = COLOR_RAIN;
        } else if (strstr(condition, "storm")) {
            icon_color = COLOR_YELLOW;
        } else if (strstr(condition, "cloud") || strstr(condition, "fog")) {
            icon_color = COLOR_TEXT_MUTED;
        } else if (!is_day) {
            icon_color = COLOR_VIOLET;
        }
        lv_obj_set_style_text_color(lbl_condition_icon, icon_color, 0);
    }

    // Actualizar texto de condicion
    if (lbl_condition_text) {
        const char* cond_text = getConditionText(condition, is_day);
        lv_label_set_text(lbl_condition_text, cond_text);
    }

    // === Temperatura (con conversion C/F) ===
    if (lbl_temp_value) {
        snprintf(buf, sizeof(buf), "%.1f°%s", toDisplayTemp(g_weather.temp_outdoor), tempUnit());
        lv_label_set_text(lbl_temp_value, buf);
    }
    if (lbl_temp_feels) {
        snprintf(buf, sizeof(buf), "Sensacion %.1f°", toDisplayTemp(g_weather.feels_like));
        lv_label_set_text(lbl_temp_feels, buf);
    }
    if (lbl_temp_max) {
        snprintf(buf, sizeof(buf), "Max %.0f°", toDisplayTemp(g_weather.temp_max));
        lv_label_set_text(lbl_temp_max, buf);
    }
    if (lbl_temp_min) {
        snprintf(buf, sizeof(buf), "Min %.0f°", toDisplayTemp(g_weather.temp_min));
        lv_label_set_text(lbl_temp_min, buf);
    }

    // === Humedad ===
    if (arc_humidity) {
        lv_arc_set_value(arc_humidity, (int)g_weather.humidity_outdoor);
    }
    if (lbl_humidity_value) {
        snprintf(buf, sizeof(buf), "%.0f%%", g_weather.humidity_outdoor);
        lv_label_set_text(lbl_humidity_value, buf);
    }

    // === Presion ===
    if (lbl_pressure_value) {
        snprintf(buf, sizeof(buf), "%.0f", g_weather.pressure_rel);
        lv_label_set_text(lbl_pressure_value, buf);
    }
    if (lbl_pressure_trend && g_compare.valid) {
        const char* arrow = g_compare.pressure_diff > 0.5 ? "↗" :
                           g_compare.pressure_diff < -0.5 ? "↘" : "→";
        lv_label_set_text(lbl_pressure_trend, arrow);
        lv_obj_set_style_text_color(lbl_pressure_trend,
            g_compare.pressure_diff > 0.5 ? COLOR_GREEN :
            g_compare.pressure_diff < -0.5 ? COLOR_RED : COLOR_TEXT_MUTED, 0);
    }
    if (lbl_pressure_diff && g_compare.valid) {
        snprintf(buf, sizeof(buf), "%+.1f (3h)", g_compare.pressure_diff);
        lv_label_set_text(lbl_pressure_diff, buf);
    }

    // === Condiciones ===
    if (lbl_wind) {
        snprintf(buf, sizeof(buf), "V: %.0f km/h %s",
                 g_weather.wind_speed, g_weather.wind_dir_cardinal);
        lv_label_set_text(lbl_wind, buf);
    }
    if (lbl_wind_gust) {
        snprintf(buf, sizeof(buf), "Rafaga: %.0f km/h", g_weather.wind_gust);
        lv_label_set_text(lbl_wind_gust, buf);
    }
    if (lbl_uv) {
        const char* uv_level = g_weather.uv < 3 ? "Bajo" :
                               g_weather.uv < 6 ? "Moderado" :
                               g_weather.uv < 8 ? "Alto" :
                               g_weather.uv < 11 ? "Muy Alto" : "Extremo";
        snprintf(buf, sizeof(buf), "* UV: %.0f (%s)", g_weather.uv, uv_level);
        lv_label_set_text(lbl_uv, buf);
    }
    if (lbl_dewpoint) {
        snprintf(buf, sizeof(buf), "T: Rocio: %.1f°C", g_weather.dewpoint);
        lv_label_set_text(lbl_dewpoint, buf);
    }
    if (lbl_rain) {
        snprintf(buf, sizeof(buf), "Lluvia: Lluvia: %.1f mm/h  ·  Hoy: %.1f mm",
                 g_weather.rain_rate, g_weather.rain_day);
        lv_label_set_text(lbl_rain, buf);
    }

    // === Alertas ===
    if (lbl_alert_status) {
        if (g_alerts.has_alerts) {
            snprintf(buf, sizeof(buf), LV_SYMBOL_WARNING " %d alerta(s)", g_alerts.alert_count);
            lv_label_set_text(lbl_alert_status, buf);
            lv_obj_set_style_text_color(lbl_alert_status, COLOR_YELLOW, 0);
        } else {
            lv_label_set_text(lbl_alert_status, "OK - Sin alertas");
            lv_obj_set_style_text_color(lbl_alert_status, COLOR_GREEN, 0);
        }
    }

    // === Tendencias ===
    if (lbl_trend_temp && g_compare.valid) {
        snprintf(buf, sizeof(buf), "T: Temp: %+.1f°C vs ayer", g_compare.temp_diff);
        lv_label_set_text(lbl_trend_temp, buf);
    }
    if (lbl_trend_humidity && g_compare.valid) {
        snprintf(buf, sizeof(buf), "H: Humedad: %+.0f%% vs ayer", g_compare.humidity_diff);
        lv_label_set_text(lbl_trend_humidity, buf);
    }

    // === Interior (BME280 local o consola) ===
    float indoor_temp, indoor_hum, indoor_pres;
    bool use_local = g_local.valid;

    if (use_local) {
        indoor_temp = g_local.temperature;
        indoor_hum = g_local.humidity;
        indoor_pres = g_local.pressure;
        if (lbl_indoor_title) lv_label_set_text(lbl_indoor_title, "* INTERIOR (BME280)");
    } else {
        indoor_temp = g_weather.temp_indoor;
        indoor_hum = g_weather.humidity_indoor;
        indoor_pres = g_weather.pressure_rel;
        if (lbl_indoor_title) lv_label_set_text(lbl_indoor_title, "* INTERIOR (Consola)");
    }

    if (lbl_indoor_temp) {
        snprintf(buf, sizeof(buf), "%.1f°%s", toDisplayTemp(indoor_temp), tempUnit());
        lv_label_set_text(lbl_indoor_temp, buf);
    }
    if (lbl_indoor_humidity) {
        snprintf(buf, sizeof(buf), "H: %.0f%%", indoor_hum);
        lv_label_set_text(lbl_indoor_humidity, buf);
    }
    if (lbl_indoor_pressure) {
        snprintf(buf, sizeof(buf), "P: %.0f hPa", indoor_pres);
        lv_label_set_text(lbl_indoor_pressure, buf);
    }
    if (lbl_indoor_diff) {
        float diff = indoor_temp - g_weather.temp_outdoor;
        snprintf(buf, sizeof(buf), "(vs exterior: %+.1f°C)", diff);
        lv_label_set_text(lbl_indoor_diff, buf);
    }

    // === Almanaque (footer) ===
    if (g_almanac.valid) {
        if (lbl_footer_sunrise) {
            snprintf(buf, sizeof(buf), "Sale: %s", g_almanac.sunrise);
            lv_label_set_text(lbl_footer_sunrise, buf);
        }
        if (lbl_footer_sunset) {
            snprintf(buf, sizeof(buf), "Pone: %s", g_almanac.sunset);
            lv_label_set_text(lbl_footer_sunset, buf);
        }
        if (lbl_footer_moon) {
            snprintf(buf, sizeof(buf), "Luna: %d%%", g_almanac.moon_illumination);
            lv_label_set_text(lbl_footer_moon, buf);
        }
    }

    // === Footer UV/AQI ===
    if (lbl_footer_uv) {
        const char* uv_level = g_weather.uv < 3 ? "Bajo" :
                               g_weather.uv < 6 ? "Mod" : "Alto";
        snprintf(buf, sizeof(buf), "UV: %.0f %s", g_weather.uv, uv_level);
        lv_label_set_text(lbl_footer_uv, buf);
    }

    // === Status indicators ===
    if (lbl_wifi_rssi) {
        lv_obj_set_style_text_color(lbl_wifi_rssi,
            g_status.wifi_connected ? COLOR_GREEN : COLOR_RED, 0);
    }
    if (lbl_server_status) {
        lv_obj_set_style_text_color(lbl_server_status,
            g_status.api_ok ? COLOR_GREEN : COLOR_RED, 0);
    }

    // === Mini-cards ===
    if (lbl_mini_hum_val) {
        snprintf(buf, sizeof(buf), "%.0f%%", g_weather.humidity_outdoor);
        lv_label_set_text(lbl_mini_hum_val, buf);
    }
    if (lbl_mini_uv_val) {
        snprintf(buf, sizeof(buf), "%.0f", g_weather.uv);
        lv_label_set_text(lbl_mini_uv_val, buf);
    }
    if (lbl_mini_pres_val) {
        snprintf(buf, sizeof(buf), "%.0f", g_weather.pressure_rel);
        lv_label_set_text(lbl_mini_pres_val, buf);
    }

    // === Panel Viento ===
    if (lbl_wind_speed) {
        snprintf(buf, sizeof(buf), "%.1f", g_weather.wind_speed);
        lv_label_set_text(lbl_wind_speed, buf);
    }
    if (lbl_wind_direction) {
        snprintf(buf, sizeof(buf), "%s %d°", g_weather.wind_dir_cardinal, (int)g_weather.wind_dir);
        lv_label_set_text(lbl_wind_direction, buf);
    }
    if (lbl_wind_dir_icon) {
        const char* dir_icon = getWindDirectionIcon((int)g_weather.wind_dir);
        lv_label_set_text(lbl_wind_dir_icon, dir_icon);
    }
    if (lbl_wind_gust_new) {
        snprintf(buf, sizeof(buf), "%.1f km/h", g_weather.wind_gust);
        lv_label_set_text(lbl_wind_gust_new, buf);
    }
    if (lbl_wind_beaufort) {
        int bf = (int)(pow(g_weather.wind_speed / 3.01, 2.0/3.0) + 0.5);
        if (bf > 12) bf = 12;
        const char* bf_names[] = {"Calma", "Ventolina", "Brisa muy debil", "Brisa debil",
            "Brisa moderada", "Brisa fresca", "Brisa fuerte", "Viento fuerte",
            "Temporal", "Temporal fuerte", "Temporal muy fuerte", "Borrasca", "Huracan"};
        snprintf(buf, sizeof(buf), "%d - %s", bf, bf_names[bf]);
        lv_label_set_text(lbl_wind_beaufort, buf);
    }

    // === Panel Lluvia ===
    if (lbl_rain_rate) {
        snprintf(buf, sizeof(buf), "%.1f", g_weather.rain_rate);
        lv_label_set_text(lbl_rain_rate, buf);
    }
    if (lbl_rain_today) {
        snprintf(buf, sizeof(buf), "%.1f", g_weather.rain_day);
        lv_label_set_text(lbl_rain_today, buf);
    }
    if (lbl_rain_month) {
        snprintf(buf, sizeof(buf), "%.0f", g_weather.rain_week);  // Usar week como approx
        lv_label_set_text(lbl_rain_month, buf);
    }

    // === Panel Sol y Luna ===
    if (g_almanac.valid) {
        if (lbl_sunrise_val) {
            lv_label_set_text(lbl_sunrise_val, g_almanac.sunrise);
        }
        if (lbl_sunset_val) {
            lv_label_set_text(lbl_sunset_val, g_almanac.sunset);
        }
        if (lbl_moon_icon) {
            bool is_waxing = strstr(g_almanac.moon_phase, "creciente") ||
                            strstr(g_almanac.moon_phase, "Creciente") ||
                            strstr(g_almanac.moon_phase, "waxing");
            const char* moon_icon = getMoonPhaseIcon(g_almanac.moon_illumination, is_waxing);
            lv_label_set_text(lbl_moon_icon, moon_icon);
        }
        if (lbl_moon_val) {
            snprintf(buf, sizeof(buf), "%s (%d%%)",
                     g_almanac.moon_phase, g_almanac.moon_illumination);
            lv_label_set_text(lbl_moon_val, buf);
        }
        if (lbl_daylight) {
            // Calcular horas de luz (aproximado)
            snprintf(buf, sizeof(buf), "Horas de luz: %d h %d min",
                     g_almanac.daylight_hours, g_almanac.daylight_minutes);
            lv_label_set_text(lbl_daylight, buf);
        }
    }

    // =========================================================================
    // NUEVO LAYOUT: Panel Principal y Cards de ubicacion
    // =========================================================================

    // === Panel Principal ===
    if (lbl_main_temp) {
        snprintf(buf, sizeof(buf), "%.1f°", toDisplayTemp(g_weather.temp_outdoor));
        lv_label_set_text(lbl_main_temp, buf);
    }
    if (lbl_main_condition_icon) {
        const char* icon = getWeatherIcon(condition, is_day);
        lv_label_set_text(lbl_main_condition_icon, icon);
        lv_color_t icon_color = COLOR_SUN;
        if (strstr(condition, "rain") || strstr(condition, "drizzle")) icon_color = COLOR_RAIN;
        else if (strstr(condition, "storm")) icon_color = COLOR_YELLOW;
        else if (strstr(condition, "cloud") || strstr(condition, "fog")) icon_color = COLOR_TEXT_MUTED;
        else if (!is_day) icon_color = COLOR_VIOLET;
        lv_obj_set_style_text_color(lbl_main_condition_icon, icon_color, 0);
    }
    if (lbl_main_condition) {
        lv_label_set_text(lbl_main_condition, getConditionText(condition, is_day));
    }
    if (lbl_main_feels) {
        snprintf(buf, sizeof(buf), "Sensacion %.1f°", toDisplayTemp(g_weather.feels_like));
        lv_label_set_text(lbl_main_feels, buf);
    }
    if (lbl_main_maxmin) {
        snprintf(buf, sizeof(buf), LV_SYMBOL_UP " %.0f°  " LV_SYMBOL_DOWN " %.0f°",
                 toDisplayTemp(g_weather.temp_max), toDisplayTemp(g_weather.temp_min));
        lv_label_set_text(lbl_main_maxmin, buf);
    }
    if (lbl_main_humidity) {
        snprintf(buf, sizeof(buf), "%.0f%%", g_weather.humidity_outdoor);
        lv_label_set_text(lbl_main_humidity, buf);
    }
    if (lbl_main_pressure) {
        snprintf(buf, sizeof(buf), "%.0f", g_weather.pressure_rel);
        lv_label_set_text(lbl_main_pressure, buf);
    }
    if (lbl_main_pressure_trend && g_compare.valid) {
        const char* arrow = g_compare.pressure_diff > 0.5 ? LV_SYMBOL_UP :
                           g_compare.pressure_diff < -0.5 ? LV_SYMBOL_DOWN : "-";
        lv_label_set_text(lbl_main_pressure_trend, arrow);
        lv_obj_set_style_text_color(lbl_main_pressure_trend,
            g_compare.pressure_diff > 0.5 ? COLOR_GREEN :
            g_compare.pressure_diff < -0.5 ? COLOR_RED : COLOR_TEXT_MUTED, 0);
    }
    if (lbl_main_wind) {
        snprintf(buf, sizeof(buf), "%.0f km/h", g_weather.wind_speed);
        lv_label_set_text(lbl_main_wind, buf);
    }
    if (lbl_main_wind_icon) {
        lv_label_set_text(lbl_main_wind_icon, getWindDirectionIcon((int)g_weather.wind_dir));
    }
    if (lbl_main_wind_dir) {
        snprintf(buf, sizeof(buf), "%s %d°", g_weather.wind_dir_cardinal, (int)g_weather.wind_dir);
        lv_label_set_text(lbl_main_wind_dir, buf);
    }
    if (lbl_main_gust) {
        snprintf(buf, sizeof(buf), "Rafagas: %.0f km/h", g_weather.wind_gust);
        lv_label_set_text(lbl_main_gust, buf);
    }
    if (lbl_main_server) {
        char time_str[10];
        struct tm ti;
        time_t t = time(nullptr);
        localtime_r(&t, &ti);
        strftime(time_str, sizeof(time_str), "%H:%M:%S", &ti);
        snprintf(buf, sizeof(buf), "Servidor: %s  |  Actualizado: %s",
                 g_status.api_ok ? "Online" : "Offline", time_str);
        lv_label_set_text(lbl_main_server, buf);
        lv_obj_set_style_text_color(lbl_main_server,
            g_status.api_ok ? COLOR_GREEN : COLOR_RED, 0);
    }

    // === Card LOCAL (BME280) ===
    if (lbl_local_temp) {
        if (g_local.valid) {
            snprintf(buf, sizeof(buf), "%.1f°", toDisplayTemp(g_local.temperature));
            lv_label_set_text(lbl_local_temp, buf);
        } else {
            lv_label_set_text(lbl_local_temp, "--.-°");
        }
    }
    if (lbl_local_maxmin && g_local.valid) {
        snprintf(buf, sizeof(buf), LV_SYMBOL_UP " %.0f°  " LV_SYMBOL_DOWN " %.0f°",
                 toDisplayTemp(g_local.temp_max), toDisplayTemp(g_local.temp_min));
        lv_label_set_text(lbl_local_maxmin, buf);
    }
    if (lbl_local_humidity) {
        if (g_local.valid) {
            snprintf(buf, sizeof(buf), "%.0f%%", g_local.humidity);
        } else {
            snprintf(buf, sizeof(buf), "--%%" );
        }
        lv_label_set_text(lbl_local_humidity, buf);
    }
    if (lbl_local_battery && g_local.valid && g_local.battery >= 0) {
        snprintf(buf, sizeof(buf), "%d%%", g_local.battery);
        lv_label_set_text(lbl_local_battery, buf);
    }

    // === Card JARDIN (HN31) ===
    extern RemoteSensorData g_jardin;
    if (lbl_jardin_temp) {
        if (g_jardin.valid) {
            snprintf(buf, sizeof(buf), "%.1f°", toDisplayTemp(g_jardin.temperature));
        } else {
            snprintf(buf, sizeof(buf), "--.-°");
        }
        lv_label_set_text(lbl_jardin_temp, buf);
    }
    if (lbl_jardin_maxmin && g_jardin.valid) {
        snprintf(buf, sizeof(buf), LV_SYMBOL_UP " %.0f°  " LV_SYMBOL_DOWN " %.0f°",
                 toDisplayTemp(g_jardin.temp_max), toDisplayTemp(g_jardin.temp_min));
        lv_label_set_text(lbl_jardin_maxmin, buf);
    }
    if (lbl_jardin_humidity) {
        if (g_jardin.valid) {
            snprintf(buf, sizeof(buf), "%.0f%%", g_jardin.humidity);
        } else {
            snprintf(buf, sizeof(buf), "--%%");
        }
        lv_label_set_text(lbl_jardin_humidity, buf);
    }
    if (lbl_jardin_battery && g_jardin.valid) {
        snprintf(buf, sizeof(buf), "%d%%", g_jardin.battery);
        lv_label_set_text(lbl_jardin_battery, buf);
    }

    // === Card REMOTO (GW110) ===
    extern RemoteGatewayData g_remoto;
    if (lbl_remoto_temp) {
        if (g_remoto.valid) {
            snprintf(buf, sizeof(buf), "%.1f°", toDisplayTemp(g_remoto.temperature));
        } else {
            snprintf(buf, sizeof(buf), "--.-°");
        }
        lv_label_set_text(lbl_remoto_temp, buf);
    }
    if (lbl_remoto_maxmin && g_remoto.valid) {
        snprintf(buf, sizeof(buf), LV_SYMBOL_UP " %.0f°  " LV_SYMBOL_DOWN " %.0f°",
                 toDisplayTemp(g_remoto.temp_max), toDisplayTemp(g_remoto.temp_min));
        lv_label_set_text(lbl_remoto_maxmin, buf);
    }
    if (lbl_remoto_humidity) {
        if (g_remoto.valid) {
            snprintf(buf, sizeof(buf), "%.0f%%", g_remoto.humidity);
        } else {
            snprintf(buf, sizeof(buf), "--%%");
        }
        lv_label_set_text(lbl_remoto_humidity, buf);
    }
    if (lbl_remoto_pressure && g_remoto.valid) {
        snprintf(buf, sizeof(buf), "%.0f hPa", g_remoto.pressure);
        lv_label_set_text(lbl_remoto_pressure, buf);
    }
}

// ============================================================================
// Refresh tema (recrea todo el dashboard)
// ============================================================================

void refreshDashboardTheme() {
    if (scr_dashboard) {
        // Actualizar fondo
        lv_obj_set_style_bg_color(scr_dashboard, COLOR_BG, 0);

        // Actualizar header
        if (header) {
            lv_obj_set_style_bg_color(header, COLOR_CARD, 0);
            lv_obj_set_style_border_color(header, COLOR_BORDER, 0);
        }

        // Actualizar footer
        if (footer) {
            lv_obj_set_style_bg_color(footer, COLOR_CARD, 0);
            lv_obj_set_style_border_color(footer, COLOR_BORDER, 0);
        }

        // Actualizar colores de texto en header
        if (lbl_header_time) lv_obj_set_style_text_color(lbl_header_time, COLOR_TEXT_PRIMARY, 0);
        if (lbl_status_text) lv_obj_set_style_text_color(lbl_status_text, COLOR_TEXT_SECONDARY, 0);

        // Actualizar botones
        if (btn_theme) lv_obj_set_style_bg_color(btn_theme, COLOR_BORDER, 0);
        if (btn_settings) lv_obj_set_style_bg_color(btn_settings, COLOR_BORDER, 0);

        // Actualizar estilos de cards
        lv_style_set_bg_color(&style_card, COLOR_CARD);
        lv_style_set_border_color(&style_card, COLOR_BORDER);

        // Forzar redibujado
        lv_obj_invalidate(scr_dashboard);
    }
}

#endif // UI_DASHBOARD_H
