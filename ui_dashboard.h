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

// ============================================================================
// Constantes de diseño
// ============================================================================

// Colores (tema claro)
#define COLOR_BG            lv_color_hex(0xF5F5F5)
#define COLOR_CARD          lv_color_hex(0xFFFFFF)
#define COLOR_BORDER        lv_color_hex(0xE0E0E0)
#define COLOR_TEXT_PRIMARY  lv_color_hex(0x2C2C2C)
#define COLOR_TEXT_SECONDARY lv_color_hex(0x666666)
#define COLOR_TEXT_MUTED    lv_color_hex(0x999999)
#define COLOR_ACCENT        lv_color_hex(0x2196F3)
#define COLOR_GREEN         lv_color_hex(0x4CAF50)
#define COLOR_YELLOW        lv_color_hex(0xFF9800)
#define COLOR_RED           lv_color_hex(0xF44336)
#define COLOR_TEMP_HOT      lv_color_hex(0xFF6B6B)
#define COLOR_TEMP_COLD     lv_color_hex(0x64B5F6)

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
static lv_obj_t *btn_settings = nullptr;
static lv_obj_t *lbl_header_time = nullptr;

// Panel Reloj
static lv_obj_t *panel_clock = nullptr;
static lv_obj_t *lbl_clock_time = nullptr;
static lv_obj_t *lbl_clock_date = nullptr;

// Panel Presión
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

// Panel Pronóstico
static lv_obj_t *panel_forecast = nullptr;
static lv_obj_t *forecast_cards[3] = {nullptr};
static lv_obj_t *lbl_fc_day[3] = {nullptr};
static lv_obj_t *lbl_fc_icon[3] = {nullptr};
static lv_obj_t *lbl_fc_temp[3] = {nullptr};
static lv_obj_t *lbl_fc_rain[3] = {nullptr};

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

void createHeader(lv_obj_t *parent) {
    header = lv_obj_create(parent);
    lv_obj_set_size(header, SCREEN_WIDTH, HEADER_HEIGHT);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_add_style(header, &style_header, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    // Status icon (green dot)
    lbl_status_icon = lv_label_create(header);
    lv_label_set_text(lbl_status_icon, "●");
    lv_obj_set_style_text_color(lbl_status_icon, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_status_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_status_icon, LV_ALIGN_LEFT_MID, 0, 0);

    // Status text
    lbl_status_text = lv_label_create(header);
    lv_label_set_text(lbl_status_text, "Online");
    lv_obj_set_style_text_color(lbl_status_text, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_status_text, &lv_font_montserrat_14, 0);
    lv_obj_align_to(lbl_status_text, lbl_status_icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);

    // WiFi RSSI
    lbl_wifi_rssi = lv_label_create(header);
    lv_label_set_text(lbl_wifi_rssi, "📶 -65 dBm");
    lv_obj_set_style_text_color(lbl_wifi_rssi, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_wifi_rssi, &lv_font_montserrat_12, 0);
    lv_obj_align_to(lbl_wifi_rssi, lbl_status_text, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    // Settings button
    btn_settings = lv_btn_create(header);
    lv_obj_set_size(btn_settings, 40, 34);
    lv_obj_align(btn_settings, LV_ALIGN_RIGHT_MID, -60, 0);
    lv_obj_set_style_bg_color(btn_settings, COLOR_BG, 0);
    lv_obj_set_style_radius(btn_settings, 8, 0);
    lv_obj_set_style_shadow_width(btn_settings, 0, 0);

    lv_obj_t *settings_icon = lv_label_create(btn_settings);
    lv_label_set_text(settings_icon, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(settings_icon, COLOR_TEXT_SECONDARY, 0);
    lv_obj_center(settings_icon);

    // Header time
    lbl_header_time = lv_label_create(header);
    lv_label_set_text(lbl_header_time, "16:48:32");
    lv_obj_set_style_text_color(lbl_header_time, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_header_time, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_header_time, LV_ALIGN_RIGHT_MID, 0, 0);
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
    lv_label_set_text(lbl_footer_uv, "☀️ UV: 3 Mod");
    lv_obj_set_style_text_color(lbl_footer_uv, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_uv, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_uv, LV_ALIGN_LEFT_MID, 0, 0);

    // AQI
    lbl_footer_aqi = lv_label_create(footer);
    lv_label_set_text(lbl_footer_aqi, "🌬️ AQI: 42");
    lv_obj_set_style_text_color(lbl_footer_aqi, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_aqi, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_aqi, LV_ALIGN_LEFT_MID, 150, 0);

    // Sunrise
    lbl_footer_sunrise = lv_label_create(footer);
    lv_label_set_text(lbl_footer_sunrise, "🌅 06:52");
    lv_obj_set_style_text_color(lbl_footer_sunrise, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_sunrise, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_sunrise, LV_ALIGN_CENTER, -120, 0);

    // Sunset
    lbl_footer_sunset = lv_label_create(footer);
    lv_label_set_text(lbl_footer_sunset, "🌇 20:14");
    lv_obj_set_style_text_color(lbl_footer_sunset, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_footer_sunset, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_footer_sunset, LV_ALIGN_CENTER, 0, 0);

    // Moon
    lbl_footer_moon = lv_label_create(footer);
    lv_label_set_text(lbl_footer_moon, "🌙 82%");
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
// Crear Panel Presión
// ============================================================================

void createPressurePanel(lv_obj_t *parent, int x, int y) {
    panel_pressure = createCard(parent, x, y, 150, 120);

    // Título
    lv_obj_t *title = lv_label_create(panel_pressure);
    lv_label_set_text(title, "PRESIÓN");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Valor
    lbl_pressure_value = lv_label_create(panel_pressure);
    lv_label_set_text(lbl_pressure_value, "1018");
    lv_obj_set_style_text_color(lbl_pressure_value, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_pressure_value, &lv_font_montserrat_28, 0);
    lv_obj_align(lbl_pressure_value, LV_ALIGN_CENTER, -10, -5);

    // Unidad
    lv_obj_t *unit = lv_label_create(panel_pressure);
    lv_label_set_text(unit, "hPa");
    lv_obj_set_style_text_color(unit, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(unit, &lv_font_montserrat_14, 0);
    lv_obj_align_to(unit, lbl_pressure_value, LV_ALIGN_OUT_RIGHT_BOTTOM, 4, 0);

    // Tendencia (flecha)
    lbl_pressure_trend = lv_label_create(panel_pressure);
    lv_label_set_text(lbl_pressure_trend, "↗");
    lv_obj_set_style_text_color(lbl_pressure_trend, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_pressure_trend, &lv_font_montserrat_20, 0);
    lv_obj_align(lbl_pressure_trend, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Diferencia
    lbl_pressure_diff = lv_label_create(panel_pressure);
    lv_label_set_text(lbl_pressure_diff, "+1.5 (3h)");
    lv_obj_set_style_text_color(lbl_pressure_diff, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_pressure_diff, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_pressure_diff, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
}

// ============================================================================
// Crear Panel Alertas/Tendencias
// ============================================================================

void createAlertsPanel(lv_obj_t *parent, int x, int y, int w) {
    panel_alerts = createCard(parent, x, y, w, 120);

    // Estado de alertas
    lbl_alert_status = lv_label_create(panel_alerts);
    lv_label_set_text(lbl_alert_status, "🟢 Sin alertas activas");
    lv_obj_set_style_text_color(lbl_alert_status, COLOR_GREEN, 0);
    lv_obj_set_style_text_font(lbl_alert_status, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_alert_status, LV_ALIGN_TOP_LEFT, 0, 0);

    // Tendencia temperatura
    lbl_trend_temp = lv_label_create(panel_alerts);
    lv_label_set_text(lbl_trend_temp, "🌡️ Temp: +0.8°C vs ayer");
    lv_obj_set_style_text_color(lbl_trend_temp, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_trend_temp, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_trend_temp, LV_ALIGN_TOP_LEFT, 0, 35);

    // Tendencia presión
    lbl_trend_pressure = lv_label_create(panel_alerts);
    lv_label_set_text(lbl_trend_pressure, "📊 Presión: -2.1 hPa/3h (bajando)");
    lv_obj_set_style_text_color(lbl_trend_pressure, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_trend_pressure, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_trend_pressure, LV_ALIGN_TOP_LEFT, 0, 58);

    // Tendencia humedad
    lbl_trend_humidity = lv_label_create(panel_alerts);
    lv_label_set_text(lbl_trend_humidity, "💧 Humedad: +5% vs ayer");
    lv_obj_set_style_text_color(lbl_trend_humidity, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_trend_humidity, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_trend_humidity, LV_ALIGN_TOP_LEFT, 0, 81);
}

// ============================================================================
// Crear Panel Temperatura
// ============================================================================

void createTemperaturePanel(lv_obj_t *parent, int x, int y) {
    panel_temp = createCard(parent, x, y, 190, 185);

    // Título
    lv_obj_t *title = lv_label_create(panel_temp);
    lv_label_set_text(title, "EXTERIOR");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Temperatura principal
    lbl_temp_value = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_value, "21.5°");
    lv_obj_set_style_text_color(lbl_temp_value, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_temp_value, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_temp_value, LV_ALIGN_TOP_MID, 0, 25);

    // Sensación térmica
    lbl_temp_feels = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_feels, "Sensación 20.2°");
    lv_obj_set_style_text_color(lbl_temp_feels, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_temp_feels, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_temp_feels, LV_ALIGN_CENTER, 0, 25);

    // Máxima
    lbl_temp_max = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_max, "▲ 26°");
    lv_obj_set_style_text_color(lbl_temp_max, COLOR_TEMP_HOT, 0);
    lv_obj_set_style_text_font(lbl_temp_max, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_temp_max, LV_ALIGN_BOTTOM_LEFT, 10, -5);

    // Mínima
    lbl_temp_min = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_min, "▼ 18°");
    lv_obj_set_style_text_color(lbl_temp_min, COLOR_TEMP_COLD, 0);
    lv_obj_set_style_text_font(lbl_temp_min, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_temp_min, LV_ALIGN_BOTTOM_RIGHT, -10, -5);
}

// ============================================================================
// Crear Panel Humedad (con gauge circular)
// ============================================================================

void createHumidityPanel(lv_obj_t *parent, int x, int y) {
    panel_humidity = createCard(parent, x, y, 150, 185);

    // Título
    lv_obj_t *title = lv_label_create(panel_humidity);
    lv_label_set_text(title, "HUMEDAD");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Arco de humedad
    arc_humidity = lv_arc_create(panel_humidity);
    lv_obj_set_size(arc_humidity, 110, 110);
    lv_arc_set_rotation(arc_humidity, 135);
    lv_arc_set_bg_angles(arc_humidity, 0, 270);
    lv_arc_set_range(arc_humidity, 0, 100);
    lv_arc_set_value(arc_humidity, 65);
    lv_obj_remove_style(arc_humidity, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_humidity, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_humidity, 12, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_humidity, 12, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_humidity, COLOR_BORDER, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_humidity, COLOR_ACCENT, LV_PART_INDICATOR);
    lv_obj_align(arc_humidity, LV_ALIGN_CENTER, 0, 10);

    // Valor en el centro del arco
    lbl_humidity_value = lv_label_create(panel_humidity);
    lv_label_set_text(lbl_humidity_value, "65%");
    lv_obj_set_style_text_color(lbl_humidity_value, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_humidity_value, &lv_font_montserrat_28, 0);
    lv_obj_align(lbl_humidity_value, LV_ALIGN_CENTER, 0, 5);

    // Label
    lbl_humidity_label = lv_label_create(panel_humidity);
    lv_label_set_text(lbl_humidity_label, "Exterior");
    lv_obj_set_style_text_color(lbl_humidity_label, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_humidity_label, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_humidity_label, LV_ALIGN_BOTTOM_MID, 0, -5);
}

// ============================================================================
// Crear Panel Condiciones
// ============================================================================

void createConditionsPanel(lv_obj_t *parent, int x, int y, int w) {
    panel_conditions = createCard(parent, x, y, w, 185);

    // Icono condición
    lbl_condition_icon = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_condition_icon, "☀️");
    lv_obj_set_style_text_font(lbl_condition_icon, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_condition_icon, LV_ALIGN_TOP_LEFT, 10, 5);

    // Texto condición
    lbl_condition_text = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_condition_text, "DESPEJADO");
    lv_obj_set_style_text_color(lbl_condition_text, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_condition_text, &lv_font_montserrat_24, 0);
    lv_obj_align(lbl_condition_text, LV_ALIGN_TOP_LEFT, 80, 15);

    // Viento
    lbl_wind = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_wind, "💨 12 km/h NE");
    lv_obj_set_style_text_color(lbl_wind, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_wind, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_wind, LV_ALIGN_TOP_LEFT, 10, 65);

    // Ráfaga
    lbl_wind_gust = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_wind_gust, "Ráfaga: 18 km/h");
    lv_obj_set_style_text_color(lbl_wind_gust, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_wind_gust, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_wind_gust, LV_ALIGN_TOP_LEFT, 150, 67);

    // UV
    lbl_uv = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_uv, "☀️ UV: 3 (Moderado)");
    lv_obj_set_style_text_color(lbl_uv, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_uv, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_uv, LV_ALIGN_TOP_LEFT, 10, 95);

    // Punto de rocío
    lbl_dewpoint = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_dewpoint, "🌡️ Rocío: 14°C");
    lv_obj_set_style_text_color(lbl_dewpoint, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_dewpoint, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_dewpoint, LV_ALIGN_TOP_LEFT, 200, 97);

    // Lluvia
    lbl_rain = lv_label_create(panel_conditions);
    lv_label_set_text(lbl_rain, "🌧️ Lluvia: 0.0 mm/h  ·  Hoy: 2.5 mm");
    lv_obj_set_style_text_color(lbl_rain, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_rain, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_rain, LV_ALIGN_TOP_LEFT, 10, 125);
}

// ============================================================================
// Crear Panel Pronóstico
// ============================================================================

void createForecastPanel(lv_obj_t *parent, int x, int y) {
    panel_forecast = createCard(parent, x, y, 350, 130);

    // Título
    lv_obj_t *title = lv_label_create(panel_forecast);
    lv_label_set_text(title, "PRONÓSTICO");
    lv_obj_set_style_text_color(title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    // 3 tarjetas de días
    const char* days[] = {"HOY", "LUN", "MAR"};
    const char* icons[] = {"☀️", "⛅", "🌧️"};
    const char* temps[] = {"26/18", "24/17", "22/16"};
    const char* rain[] = {"0%", "20%", "60%"};

    for (int i = 0; i < 3; i++) {
        forecast_cards[i] = lv_obj_create(panel_forecast);
        lv_obj_set_size(forecast_cards[i], 100, 95);
        lv_obj_set_pos(forecast_cards[i], 10 + i * 110, 22);
        lv_obj_set_style_bg_color(forecast_cards[i], COLOR_BG, 0);
        lv_obj_set_style_radius(forecast_cards[i], 12, 0);
        lv_obj_set_style_border_width(forecast_cards[i], 0, 0);
        lv_obj_set_style_pad_all(forecast_cards[i], 8, 0);
        lv_obj_clear_flag(forecast_cards[i], LV_OBJ_FLAG_SCROLLABLE);

        // Día
        lbl_fc_day[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_day[i], days[i]);
        lv_obj_set_style_text_color(lbl_fc_day[i], COLOR_TEXT_SECONDARY, 0);
        lv_obj_set_style_text_font(lbl_fc_day[i], &lv_font_montserrat_12, 0);
        lv_obj_align(lbl_fc_day[i], LV_ALIGN_TOP_MID, 0, 0);

        // Icono
        lbl_fc_icon[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_icon[i], icons[i]);
        lv_obj_set_style_text_font(lbl_fc_icon[i], &lv_font_montserrat_24, 0);
        lv_obj_align(lbl_fc_icon[i], LV_ALIGN_CENTER, 0, -5);

        // Temperatura
        lbl_fc_temp[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_temp[i], temps[i]);
        lv_obj_set_style_text_color(lbl_fc_temp[i], COLOR_TEXT_PRIMARY, 0);
        lv_obj_set_style_text_font(lbl_fc_temp[i], &lv_font_montserrat_14, 0);
        lv_obj_align(lbl_fc_temp[i], LV_ALIGN_BOTTOM_MID, 0, -15);

        // Probabilidad lluvia
        lbl_fc_rain[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_rain[i], rain[i]);
        lv_obj_set_style_text_color(lbl_fc_rain[i], COLOR_TEMP_COLD, 0);
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
    lv_label_set_text(lbl_indoor_title, "🏠 INTERIOR");
    lv_obj_set_style_text_color(lbl_indoor_title, COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_indoor_title, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_indoor_title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Temperatura interior
    lbl_indoor_temp = lv_label_create(panel_indoor);
    lv_label_set_text(lbl_indoor_temp, "22.3°C");
    lv_obj_set_style_text_color(lbl_indoor_temp, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_indoor_temp, &lv_font_montserrat_28, 0);
    lv_obj_align(lbl_indoor_temp, LV_ALIGN_TOP_LEFT, 10, 30);

    // Humedad interior
    lbl_indoor_humidity = lv_label_create(panel_indoor);
    lv_label_set_text(lbl_indoor_humidity, "💧 58%");
    lv_obj_set_style_text_color(lbl_indoor_humidity, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(lbl_indoor_humidity, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_indoor_humidity, LV_ALIGN_TOP_LEFT, 130, 35);

    // Presión interior
    lbl_indoor_pressure = lv_label_create(panel_indoor);
    lv_label_set_text(lbl_indoor_pressure, "📊 1018 hPa");
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
// Crear Dashboard completo
// ============================================================================

void createDashboard() {
    initDashboardStyles();

    // Crear pantalla base
    scr_dashboard = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_dashboard, COLOR_BG, 0);
    lv_obj_clear_flag(scr_dashboard, LV_OBJ_FLAG_SCROLLABLE);

    // Header y Footer
    createHeader(scr_dashboard);
    createFooter(scr_dashboard);

    // Área de contenido (entre header y footer)
    int content_y = HEADER_HEIGHT + GAP;
    int content_h = SCREEN_HEIGHT - HEADER_HEIGHT - FOOTER_HEIGHT - GAP * 2;

    // Fila 1: Reloj + Presión + Alertas
    createClockPanel(scr_dashboard, GAP, content_y);
    createPressurePanel(scr_dashboard, GAP + 190 + GAP, content_y);
    createAlertsPanel(scr_dashboard, GAP + 190 + GAP + 150 + GAP, content_y,
                      SCREEN_WIDTH - (GAP + 190 + GAP + 150 + GAP) - GAP);

    // Fila 2: Temperatura + Humedad + Condiciones
    int row2_y = content_y + 120 + GAP;
    createTemperaturePanel(scr_dashboard, GAP, row2_y);
    createHumidityPanel(scr_dashboard, GAP + 190 + GAP, row2_y);
    createConditionsPanel(scr_dashboard, GAP + 190 + GAP + 150 + GAP, row2_y,
                          SCREEN_WIDTH - (GAP + 190 + GAP + 150 + GAP) - GAP);

    // Fila 3: Pronóstico + Interior
    int row3_y = row2_y + 185 + GAP;
    createForecastPanel(scr_dashboard, GAP, row3_y);
    createIndoorPanel(scr_dashboard, GAP + 350 + GAP, row3_y,
                      SCREEN_WIDTH - (GAP + 350 + GAP) - GAP);

    // Cargar pantalla
    lv_scr_load(scr_dashboard);

    Serial.println("[UI] Dashboard creado");
}

// ============================================================================
// Actualizar datos en el dashboard
// ============================================================================

void updateDashboardTime() {
    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);

    // Hora
    char time_str[10];
    strftime(time_str, sizeof(time_str), "%H:%M", &timeinfo);
    if (lbl_clock_time) lv_label_set_text(lbl_clock_time, time_str);

    // Hora en header (con segundos)
    char time_full[10];
    strftime(time_full, sizeof(time_full), "%H:%M:%S", &timeinfo);
    if (lbl_header_time) lv_label_set_text(lbl_header_time, time_full);

    // Fecha
    char date_str[20];
    const char* dias[] = {"Dom", "Lun", "Mar", "Mié", "Jue", "Vie", "Sáb"};
    const char* meses[] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun",
                           "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
    snprintf(date_str, sizeof(date_str), "%s %d %s",
             dias[timeinfo.tm_wday], timeinfo.tm_mday, meses[timeinfo.tm_mon]);
    if (lbl_clock_date) lv_label_set_text(lbl_clock_date, date_str);
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
    if (lbl_wifi_rssi) {
        snprintf(buf, sizeof(buf), "📶 %d dBm", g_status.wifi_rssi);
        lv_label_set_text(lbl_wifi_rssi, buf);
    }

    // === Temperatura ===
    if (lbl_temp_value) {
        snprintf(buf, sizeof(buf), "%.1f°", g_weather.temp_outdoor);
        lv_label_set_text(lbl_temp_value, buf);
    }
    if (lbl_temp_feels) {
        snprintf(buf, sizeof(buf), "Sensación %.1f°", g_weather.feels_like);
        lv_label_set_text(lbl_temp_feels, buf);
    }
    if (lbl_temp_max) {
        snprintf(buf, sizeof(buf), "▲ %.0f°", g_weather.temp_max);
        lv_label_set_text(lbl_temp_max, buf);
    }
    if (lbl_temp_min) {
        snprintf(buf, sizeof(buf), "▼ %.0f°", g_weather.temp_min);
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

    // === Presión ===
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
        snprintf(buf, sizeof(buf), "💨 %.0f km/h %s",
                 g_weather.wind_speed, g_weather.wind_dir_cardinal);
        lv_label_set_text(lbl_wind, buf);
    }
    if (lbl_wind_gust) {
        snprintf(buf, sizeof(buf), "Ráfaga: %.0f km/h", g_weather.wind_gust);
        lv_label_set_text(lbl_wind_gust, buf);
    }
    if (lbl_uv) {
        const char* uv_level = g_weather.uv < 3 ? "Bajo" :
                               g_weather.uv < 6 ? "Moderado" :
                               g_weather.uv < 8 ? "Alto" :
                               g_weather.uv < 11 ? "Muy Alto" : "Extremo";
        snprintf(buf, sizeof(buf), "☀️ UV: %.0f (%s)", g_weather.uv, uv_level);
        lv_label_set_text(lbl_uv, buf);
    }
    if (lbl_dewpoint) {
        snprintf(buf, sizeof(buf), "🌡️ Rocío: %.1f°C", g_weather.dewpoint);
        lv_label_set_text(lbl_dewpoint, buf);
    }
    if (lbl_rain) {
        snprintf(buf, sizeof(buf), "🌧️ Lluvia: %.1f mm/h  ·  Hoy: %.1f mm",
                 g_weather.rain_rate, g_weather.rain_day);
        lv_label_set_text(lbl_rain, buf);
    }

    // === Alertas ===
    if (lbl_alert_status) {
        if (g_alerts.has_alerts) {
            snprintf(buf, sizeof(buf), "⚠️ %d alerta(s) activa(s)", g_alerts.alert_count);
            lv_label_set_text(lbl_alert_status, buf);
            lv_obj_set_style_text_color(lbl_alert_status, COLOR_YELLOW, 0);
        } else {
            lv_label_set_text(lbl_alert_status, "🟢 Sin alertas activas");
            lv_obj_set_style_text_color(lbl_alert_status, COLOR_GREEN, 0);
        }
    }

    // === Tendencias ===
    if (lbl_trend_temp && g_compare.valid) {
        snprintf(buf, sizeof(buf), "🌡️ Temp: %+.1f°C vs ayer", g_compare.temp_diff);
        lv_label_set_text(lbl_trend_temp, buf);
    }
    if (lbl_trend_humidity && g_compare.valid) {
        snprintf(buf, sizeof(buf), "💧 Humedad: %+.0f%% vs ayer", g_compare.humidity_diff);
        lv_label_set_text(lbl_trend_humidity, buf);
    }

    // === Interior (BME280 local o consola) ===
    float indoor_temp, indoor_hum, indoor_pres;
    bool use_local = g_local.valid;

    if (use_local) {
        indoor_temp = g_local.temperature;
        indoor_hum = g_local.humidity;
        indoor_pres = g_local.pressure;
        if (lbl_indoor_title) lv_label_set_text(lbl_indoor_title, "🏠 INTERIOR (BME280)");
    } else {
        indoor_temp = g_weather.temp_indoor;
        indoor_hum = g_weather.humidity_indoor;
        indoor_pres = g_weather.pressure_rel;
        if (lbl_indoor_title) lv_label_set_text(lbl_indoor_title, "🏠 INTERIOR (Consola)");
    }

    if (lbl_indoor_temp) {
        snprintf(buf, sizeof(buf), "%.1f°C", indoor_temp);
        lv_label_set_text(lbl_indoor_temp, buf);
    }
    if (lbl_indoor_humidity) {
        snprintf(buf, sizeof(buf), "💧 %.0f%%", indoor_hum);
        lv_label_set_text(lbl_indoor_humidity, buf);
    }
    if (lbl_indoor_pressure) {
        snprintf(buf, sizeof(buf), "📊 %.0f hPa", indoor_pres);
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
            snprintf(buf, sizeof(buf), "🌅 %s", g_almanac.sunrise);
            lv_label_set_text(lbl_footer_sunrise, buf);
        }
        if (lbl_footer_sunset) {
            snprintf(buf, sizeof(buf), "🌇 %s", g_almanac.sunset);
            lv_label_set_text(lbl_footer_sunset, buf);
        }
        if (lbl_footer_moon) {
            snprintf(buf, sizeof(buf), "🌙 %d%%", g_almanac.moon_illumination);
            lv_label_set_text(lbl_footer_moon, buf);
        }
    }

    // === Footer UV/AQI ===
    if (lbl_footer_uv) {
        const char* uv_level = g_weather.uv < 3 ? "Bajo" :
                               g_weather.uv < 6 ? "Mod" : "Alto";
        snprintf(buf, sizeof(buf), "☀️ UV: %.0f %s", g_weather.uv, uv_level);
        lv_label_set_text(lbl_footer_uv, buf);
    }
}

#endif // UI_DASHBOARD_H
