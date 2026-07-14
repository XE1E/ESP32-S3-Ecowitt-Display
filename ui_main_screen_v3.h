/**
 * ui_main_screen_v3.h - Main Dashboard Screen V3
 *
 * Diseño exacto basado en imagen de referencia
 * Layout: 1024x600, modo claro, estilo widget cards
 */

#ifndef UI_MAIN_SCREEN_V3_H
#define UI_MAIN_SCREEN_V3_H

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include "config.h"
#include "config_data.h"
#include "weather_icon_data.h"
#include "trend_data.h"
#include "data_watchdog.h"

extern void switchToTabsScreen();

// ============================================================================
// WiFi Signal Strength Helper
// ============================================================================

const char* getWiFiStrengthText(int rssi) {
    if (rssi >= -50) return "EXCELENTE";
    if (rssi >= -60) return "MUY BUENA";
    if (rssi >= -70) return "BUENA";
    if (rssi >= -80) return "REGULAR";
    if (rssi >= -90) return "DEBIL";
    return "MUY DEBIL";
}

lv_color_t getWiFiStrengthColor(int rssi) {
    if (rssi >= -50) return lv_color_hex(0x4CAF50);  // Green
    if (rssi >= -60) return lv_color_hex(0x8BC34A);  // Light green
    if (rssi >= -70) return lv_color_hex(0xFFEB3B);  // Yellow
    if (rssi >= -80) return lv_color_hex(0xFF9800);  // Orange
    return lv_color_hex(0xF44336);  // Red
}

// ============================================================================
// Night Description Generator
// ============================================================================

void generateNightDescription(char* buf, size_t size, float nightTemp, int humidity, int cloudPercent) {
    const char* clarity;
    const char* comfort;

    // Sky clarity based on clouds
    if (cloudPercent < 20) {
        clarity = "DESPEJADA";
    } else if (cloudPercent < 50) {
        clarity = "ALGO NUBLADA";
    } else if (cloudPercent < 80) {
        clarity = "NUBLADA";
    } else {
        clarity = "MUY NUBLADA";
    }

    // Comfort based on temperature
    if (nightTemp < 10) {
        comfort = "FRIA";
    } else if (nightTemp < 18) {
        comfort = "FRESCA";
    } else if (nightTemp < 24) {
        comfort = "TEMPLADA";
    } else if (nightTemp < 28) {
        comfort = "CALIDA";
    } else {
        comfort = "CALUROSA";
    }

    snprintf(buf, size, "NOCHE %s Y %s", clarity, comfort);
}

// ============================================================================
// Screen & Panel References
// ============================================================================

static lv_obj_t *main_screen_v3 = nullptr;

// Fixed layers (persist across screens)
static lv_obj_t *fixed_header = nullptr;
static lv_obj_t *fixed_footer = nullptr;
static bool fixed_layers_created = false;

// Row 1 Panels
static lv_obj_t *panel_clock = nullptr;
static lv_obj_t *panel_pressure = nullptr;
static lv_obj_t *panel_status_trends = nullptr;

// Row 2 Panels
static lv_obj_t *panel_temp = nullptr;
static lv_obj_t *panel_humidity = nullptr;
static lv_obj_t *panel_weather = nullptr;

// Row 3 Panels
static lv_obj_t *panel_forecast = nullptr;

// Clock elements
static lv_obj_t *lbl_clock_time = nullptr;
static lv_obj_t *lbl_clock_date = nullptr;

// Pressure elements
static lv_obj_t *lbl_pressure_title = nullptr;
static lv_obj_t *lbl_pressure_value = nullptr;

// Status elements (inside trends panel)
static lv_obj_t *lbl_wifi_icon = nullptr;
static lv_obj_t *lbl_wifi_text = nullptr;
static lv_obj_t *lbl_data_icon = nullptr;
static lv_obj_t *lbl_data_text = nullptr;
static lv_obj_t *lbl_battery_icon = nullptr;
static lv_obj_t *lbl_battery_text = nullptr;

// Trends elements
static lv_obj_t *lbl_trend_press_arrow = nullptr;
static lv_obj_t *lbl_trend_press = nullptr;
static lv_obj_t *lbl_trend_temp_arrow = nullptr;
static lv_obj_t *lbl_trend_temp = nullptr;
static lv_obj_t *lbl_trend_temp_arrow2 = nullptr;
static lv_obj_t *lbl_trend_hum_icon = nullptr;
static lv_obj_t *lbl_trend_hum = nullptr;

// Temperature elements
static lv_obj_t *lbl_temp_value = nullptr;
static lv_obj_t *lbl_temp_label = nullptr;

// Humidity gauge elements
static lv_obj_t *arc_hum_green = nullptr;
static lv_obj_t *arc_hum_yellow = nullptr;
static lv_obj_t *arc_hum_orange = nullptr;
static lv_obj_t *arc_hum_red = nullptr;
static lv_obj_t *arc_hum_indicator = nullptr;
static lv_obj_t *lbl_hum_value = nullptr;
static lv_obj_t *lbl_hum_label = nullptr;

// Weather panel elements
static lv_obj_t *img_weather_icon = nullptr;
static lv_obj_t *lbl_sky_condition = nullptr;
static lv_obj_t *lbl_sky_night = nullptr;
static lv_obj_t *lbl_sunrise = nullptr;
static lv_obj_t *lbl_sunset = nullptr;

// Forecast elements (3 days)
static lv_obj_t *forecast_cards[3] = {nullptr};
static lv_obj_t *lbl_fc_day[3] = {nullptr};
static lv_obj_t *img_fc_icon[3] = {nullptr};
static lv_obj_t *lbl_fc_temp[3] = {nullptr};

// Footer elements
static lv_obj_t *lbl_uv = nullptr;
static lv_obj_t *lbl_ica = nullptr;
static lv_obj_t *lbl_visibility = nullptr;
static lv_obj_t *lbl_clouds = nullptr;

// ============================================================================
// Styles
// ============================================================================

static lv_style_t style_v3_bg;
static lv_style_t style_v3_card;
static lv_style_t style_v3_card_inner;
static lv_style_t style_v3_footer;
static bool styles_v3_init = false;

// Colors matching reference image
#define V3_BG_COLOR         lv_color_hex(0xE8E8E8)  // Light gray background
#define V3_CARD_COLOR       lv_color_hex(0xFAFAFA)  // White card
#define V3_CARD_INNER       lv_color_hex(0xF0F0F0)  // Inner card (forecast)
#define V3_FOOTER_COLOR     lv_color_hex(0x2C3E50)  // Dark footer
#define V3_TEXT_DARK        lv_color_hex(0x1A1A1A)  // Main text
#define V3_TEXT_GRAY        lv_color_hex(0x555555)  // Secondary text
#define V3_TEXT_LIGHT       lv_color_hex(0x888888)  // Subtle text
#define V3_BLUE             lv_color_hex(0x2196F3)  // Blue
#define V3_RED              lv_color_hex(0xC62828)  // Red
#define V3_GREEN            lv_color_hex(0x4CAF50)  // Green
#define V3_ORANGE           lv_color_hex(0xFF9800)  // Orange
#define V3_YELLOW           lv_color_hex(0xFFEB3B)  // Yellow

void initStylesV3() {
    if (styles_v3_init) return;

    // Screen background
    lv_style_init(&style_v3_bg);
    lv_style_set_bg_color(&style_v3_bg, V3_BG_COLOR);

    // Card style
    lv_style_init(&style_v3_card);
    lv_style_set_bg_color(&style_v3_card, V3_CARD_COLOR);
    lv_style_set_bg_opa(&style_v3_card, LV_OPA_COVER);
    lv_style_set_radius(&style_v3_card, 16);
    lv_style_set_border_width(&style_v3_card, 0);
    lv_style_set_shadow_width(&style_v3_card, 15);
    lv_style_set_shadow_color(&style_v3_card, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&style_v3_card, LV_OPA_10);
    lv_style_set_shadow_ofs_x(&style_v3_card, 2);
    lv_style_set_shadow_ofs_y(&style_v3_card, 4);
    lv_style_set_pad_all(&style_v3_card, 12);

    // Inner card (forecast days)
    lv_style_init(&style_v3_card_inner);
    lv_style_set_bg_color(&style_v3_card_inner, V3_CARD_INNER);
    lv_style_set_bg_opa(&style_v3_card_inner, LV_OPA_COVER);
    lv_style_set_radius(&style_v3_card_inner, 12);
    lv_style_set_border_width(&style_v3_card_inner, 0);
    lv_style_set_shadow_width(&style_v3_card_inner, 5);
    lv_style_set_shadow_opa(&style_v3_card_inner, LV_OPA_10);
    lv_style_set_pad_all(&style_v3_card_inner, 8);

    // Footer style
    lv_style_init(&style_v3_footer);
    lv_style_set_bg_color(&style_v3_footer, V3_FOOTER_COLOR);
    lv_style_set_bg_opa(&style_v3_footer, LV_OPA_COVER);
    lv_style_set_radius(&style_v3_footer, 0);
    lv_style_set_pad_hor(&style_v3_footer, 20);
    lv_style_set_pad_ver(&style_v3_footer, 10);

    styles_v3_init = true;
}

// ============================================================================
// Helper: Create Card
// ============================================================================

lv_obj_t* createCardV3(lv_obj_t *parent, int x, int y, int w, int h) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_add_style(card, &style_v3_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    return card;
}

// ============================================================================
// Clock Panel - Row 1, Left
// Position: (15, 50), Size: (200, 165) - adjusted for fixed header
// ============================================================================

void createClockPanelV3(lv_obj_t *parent) {
    panel_clock = createCardV3(parent, 15, 50, 200, 165);

    // Time - large dot-matrix style font
    lbl_clock_time = lv_label_create(panel_clock);
    lv_label_set_text(lbl_clock_time, "16:48");
    lv_obj_set_style_text_font(lbl_clock_time, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(lbl_clock_time, V3_TEXT_DARK, 0);
    lv_obj_set_style_text_letter_space(lbl_clock_time, 3, 0);
    lv_obj_align(lbl_clock_time, LV_ALIGN_TOP_MID, 0, 20);

    // Date
    lbl_clock_date = lv_label_create(panel_clock);
    lv_label_set_text(lbl_clock_date, "12 SEP 2024");
    lv_obj_set_style_text_font(lbl_clock_date, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_clock_date, V3_TEXT_GRAY, 0);
    lv_obj_align(lbl_clock_date, LV_ALIGN_BOTTOM_MID, 0, -15);
}

// ============================================================================
// Pressure Panel - Row 1, Center-Left
// Position: (225, 50), Size: (185, 165) - adjusted for fixed header
// ============================================================================

void createPressurePanelV3(lv_obj_t *parent) {
    panel_pressure = createCardV3(parent, 225, 50, 185, 165);

    // Title
    lbl_pressure_title = lv_label_create(panel_pressure);
    lv_label_set_text_static(lbl_pressure_title, "PRESION (mb):");
    lv_obj_set_style_text_font(lbl_pressure_title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_pressure_title, V3_TEXT_GRAY, 0);
    lv_obj_align(lbl_pressure_title, LV_ALIGN_TOP_MID, 0, 15);

    // Value
    lbl_pressure_value = lv_label_create(panel_pressure);
    lv_label_set_text(lbl_pressure_value, "1013.2mb");
    lv_obj_set_style_text_font(lbl_pressure_value, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lbl_pressure_value, V3_TEXT_DARK, 0);
    lv_obj_align(lbl_pressure_value, LV_ALIGN_CENTER, 0, 10);
}

// ============================================================================
// Trends Panel - Row 1, Right
// Position: (420, 50), Size: (590, 165) - adjusted for fixed header
// Contains: 3 trend lines (WiFi/Battery moved to fixed header)
// ============================================================================

void createTrendsPanelV3(lv_obj_t *parent) {
    panel_status_trends = createCardV3(parent, 420, 50, 590, 165);

    // === Trend lines ===
    int y_start = 15;
    int row_h = 45;

    // Trend Pressure (blue arrow down)
    lbl_trend_press_arrow = lv_label_create(panel_status_trends);
    lv_label_set_text(lbl_trend_press_arrow, "\xE2\x96\xBC");  // ▼
    lv_obj_set_style_text_color(lbl_trend_press_arrow, V3_BLUE, 0);
    lv_obj_set_style_text_font(lbl_trend_press_arrow, &lv_font_montserrat_20, 0);
    lv_obj_set_pos(lbl_trend_press_arrow, 10, y_start);

    lbl_trend_press = lv_label_create(panel_status_trends);
    lv_label_set_text(lbl_trend_press, "TEND. PRESION: -1.2 mb/3h");
    lv_obj_set_style_text_font(lbl_trend_press, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_trend_press, V3_TEXT_DARK, 0);
    lv_obj_set_pos(lbl_trend_press, 40, y_start + 2);

    // Trend Temp (red arrow up + diagonal)
    lbl_trend_temp_arrow = lv_label_create(panel_status_trends);
    lv_label_set_text(lbl_trend_temp_arrow, "\xE2\x96\xB2");  // ▲
    lv_obj_set_style_text_color(lbl_trend_temp_arrow, V3_RED, 0);
    lv_obj_set_style_text_font(lbl_trend_temp_arrow, &lv_font_montserrat_20, 0);
    lv_obj_set_pos(lbl_trend_temp_arrow, 10, y_start + row_h);

    lbl_trend_temp = lv_label_create(panel_status_trends);
    lv_label_set_text(lbl_trend_temp, "TEND. TEMP: +0.5C/3h");
    lv_obj_set_style_text_font(lbl_trend_temp, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_trend_temp, V3_TEXT_DARK, 0);
    lv_obj_set_pos(lbl_trend_temp, 40, y_start + row_h + 2);

    // Diagonal arrow indicator
    lbl_trend_temp_arrow2 = lv_label_create(panel_status_trends);
    lv_label_set_text(lbl_trend_temp_arrow2, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(lbl_trend_temp_arrow2, V3_RED, 0);
    lv_obj_set_style_transform_angle(lbl_trend_temp_arrow2, -450, 0);
    lv_obj_set_pos(lbl_trend_temp_arrow2, 320, y_start + row_h - 5);

    // Trend Humidity (water drop icon)
    lbl_trend_hum_icon = lv_label_create(panel_status_trends);
    lv_label_set_text(lbl_trend_hum_icon, LV_SYMBOL_TINT);
    lv_obj_set_style_text_color(lbl_trend_hum_icon, V3_BLUE, 0);
    lv_obj_set_style_text_font(lbl_trend_hum_icon, &lv_font_montserrat_18, 0);
    lv_obj_set_pos(lbl_trend_hum_icon, 12, y_start + row_h * 2);

    lbl_trend_hum = lv_label_create(panel_status_trends);
    lv_label_set_text(lbl_trend_hum, "TEND. HUMEDAD: +2% / 3h");
    lv_obj_set_style_text_font(lbl_trend_hum, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_trend_hum, V3_TEXT_DARK, 0);
    lv_obj_set_pos(lbl_trend_hum, 40, y_start + row_h * 2 + 2);
}

// ============================================================================
// Temperature Panel - Row 2, Left
// Position: (15, 225), Size: (200, 175) - adjusted for fixed header
// ============================================================================

void createTempPanelV3(lv_obj_t *parent) {
    panel_temp = createCardV3(parent, 15, 225, 200, 175);

    // Large temperature
    lbl_temp_value = lv_label_create(panel_temp);
    lv_label_set_text(lbl_temp_value, "21.5°C");
    lv_obj_set_style_text_font(lbl_temp_value, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(lbl_temp_value, V3_TEXT_DARK, 0);
    lv_obj_align(lbl_temp_value, LV_ALIGN_CENTER, 0, -10);

    // TEMP label
    lbl_temp_label = lv_label_create(panel_temp);
    lv_label_set_text_static(lbl_temp_label, "TEMP");
    lv_obj_set_style_text_font(lbl_temp_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_temp_label, V3_TEXT_GRAY, 0);
    lv_obj_align(lbl_temp_label, LV_ALIGN_BOTTOM_MID, 0, -15);
}

// ============================================================================
// Humidity Gauge Panel - Row 2, Center-Left
// Position: (225, 225), Size: (185, 175) - adjusted for fixed header
// Multi-colored gauge (green -> yellow -> orange -> red)
// ============================================================================

void createHumidityPanelV3(lv_obj_t *parent) {
    panel_humidity = createCardV3(parent, 225, 225, 185, 175);

    int arc_size = 140;
    int arc_width = 12;
    int arc_x = 10;
    int arc_y = 5;

    // Create colored segments for gauge background
    // Green segment (0-25% of arc = low humidity zone)
    arc_hum_green = lv_arc_create(panel_humidity);
    lv_obj_set_size(arc_hum_green, arc_size, arc_size);
    lv_arc_set_rotation(arc_hum_green, 135);
    lv_arc_set_bg_angles(arc_hum_green, 0, 68);  // ~25% of 270
    lv_arc_set_range(arc_hum_green, 0, 100);
    lv_arc_set_value(arc_hum_green, 0);
    lv_obj_remove_style(arc_hum_green, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_hum_green, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_hum_green, arc_width, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_hum_green, V3_GREEN, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arc_hum_green, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_pos(arc_hum_green, arc_x, arc_y);

    // Yellow segment (25-50%)
    arc_hum_yellow = lv_arc_create(panel_humidity);
    lv_obj_set_size(arc_hum_yellow, arc_size, arc_size);
    lv_arc_set_rotation(arc_hum_yellow, 135 + 68);
    lv_arc_set_bg_angles(arc_hum_yellow, 0, 67);
    lv_arc_set_range(arc_hum_yellow, 0, 100);
    lv_arc_set_value(arc_hum_yellow, 0);
    lv_obj_remove_style(arc_hum_yellow, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_hum_yellow, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_hum_yellow, arc_width, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_hum_yellow, V3_YELLOW, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arc_hum_yellow, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_pos(arc_hum_yellow, arc_x, arc_y);

    // Orange segment (50-75%)
    arc_hum_orange = lv_arc_create(panel_humidity);
    lv_obj_set_size(arc_hum_orange, arc_size, arc_size);
    lv_arc_set_rotation(arc_hum_orange, 135 + 135);
    lv_arc_set_bg_angles(arc_hum_orange, 0, 67);
    lv_arc_set_range(arc_hum_orange, 0, 100);
    lv_arc_set_value(arc_hum_orange, 0);
    lv_obj_remove_style(arc_hum_orange, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_hum_orange, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_hum_orange, arc_width, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_hum_orange, V3_ORANGE, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arc_hum_orange, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_pos(arc_hum_orange, arc_x, arc_y);

    // Red segment (75-100%)
    arc_hum_red = lv_arc_create(panel_humidity);
    lv_obj_set_size(arc_hum_red, arc_size, arc_size);
    lv_arc_set_rotation(arc_hum_red, 135 + 202);
    lv_arc_set_bg_angles(arc_hum_red, 0, 68);
    lv_arc_set_range(arc_hum_red, 0, 100);
    lv_arc_set_value(arc_hum_red, 0);
    lv_obj_remove_style(arc_hum_red, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_hum_red, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_hum_red, arc_width, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_hum_red, V3_RED, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arc_hum_red, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_pos(arc_hum_red, arc_x, arc_y);

    // Indicator arc (shows current value)
    arc_hum_indicator = lv_arc_create(panel_humidity);
    lv_obj_set_size(arc_hum_indicator, arc_size - 20, arc_size - 20);
    lv_arc_set_rotation(arc_hum_indicator, 135);
    lv_arc_set_bg_angles(arc_hum_indicator, 0, 270);
    lv_arc_set_range(arc_hum_indicator, 0, 100);
    lv_arc_set_value(arc_hum_indicator, 65);
    lv_obj_remove_style(arc_hum_indicator, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc_hum_indicator, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(arc_hum_indicator, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc_hum_indicator, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc_hum_indicator, lv_color_hex(0xDDDDDD), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc_hum_indicator, V3_TEXT_DARK, LV_PART_INDICATOR);
    lv_obj_set_pos(arc_hum_indicator, arc_x + 10, arc_y + 10);

    // Humidity value
    lbl_hum_value = lv_label_create(panel_humidity);
    lv_label_set_text(lbl_hum_value, "65%");
    lv_obj_set_style_text_font(lbl_hum_value, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(lbl_hum_value, V3_TEXT_DARK, 0);
    lv_obj_align(lbl_hum_value, LV_ALIGN_CENTER, 0, 0);

    // HUMIDITY label with water drop
    lbl_hum_label = lv_label_create(panel_humidity);
    static const char hum_label_text[] = "HUMIDITY " LV_SYMBOL_TINT;
    lv_label_set_text_static(lbl_hum_label, hum_label_text);
    lv_obj_set_style_text_font(lbl_hum_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_hum_label, V3_TEXT_GRAY, 0);
    lv_obj_align(lbl_hum_label, LV_ALIGN_BOTTOM_MID, 0, -8);
}

// ============================================================================
// Weather Panel - Row 2, Right (spans into Row 3)
// Position: (420, 225), Size: (590, 265) - adjusted for fixed header/footer
// Large sun icon on right, text on left
// ============================================================================

void createWeatherPanelV3(lv_obj_t *parent) {
    panel_weather = createCardV3(parent, 420, 225, 590, 265);

    // Large weather icon on right side
    img_weather_icon = lv_img_create(panel_weather);
    lv_img_set_src(img_weather_icon, &img_sunny);
    lv_img_set_zoom(img_weather_icon, 768);  // 3x scale (32x32 -> ~96x96)
    lv_obj_align(img_weather_icon, LV_ALIGN_RIGHT_MID, -80, -20);

    // Sky condition line 1
    lbl_sky_condition = lv_label_create(panel_weather);
    lv_label_set_text(lbl_sky_condition, "CIELO: DESPEJADO Y SOLEADO");
    lv_obj_set_style_text_font(lbl_sky_condition, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_sky_condition, V3_TEXT_DARK, 0);
    lv_obj_set_pos(lbl_sky_condition, 15, 15);

    // Sky condition line 2
    lbl_sky_night = lv_label_create(panel_weather);
    lv_label_set_text(lbl_sky_night, "NOCHE DESPEJADA Y TEMPLADA");
    lv_obj_set_style_text_font(lbl_sky_night, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_sky_night, V3_TEXT_GRAY, 0);
    lv_obj_set_pos(lbl_sky_night, 15, 45);

    // Sunrise
    lv_obj_t *sunrise_icon = lv_label_create(panel_weather);
    lv_label_set_text(sunrise_icon, "\xE2\x96\xB2");  // ▲
    lv_obj_set_style_text_color(sunrise_icon, V3_ORANGE, 0);
    lv_obj_set_style_text_font(sunrise_icon, &lv_font_montserrat_18, 0);
    lv_obj_set_pos(sunrise_icon, 15, 200);

    lbl_sunrise = lv_label_create(panel_weather);
    lv_label_set_text(lbl_sunrise, "AMANECE: 07:15");
    lv_obj_set_style_text_font(lbl_sunrise, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_sunrise, V3_TEXT_DARK, 0);
    lv_obj_set_pos(lbl_sunrise, 42, 200);

    // Sunset
    lv_obj_t *sunset_icon = lv_label_create(panel_weather);
    lv_label_set_text(sunset_icon, "\xE2\x96\xBC");  // ▼
    lv_obj_set_style_text_color(sunset_icon, V3_ORANGE, 0);
    lv_obj_set_style_text_font(sunset_icon, &lv_font_montserrat_18, 0);
    lv_obj_set_pos(sunset_icon, 230, 200);

    lbl_sunset = lv_label_create(panel_weather);
    lv_label_set_text(lbl_sunset, "ANOCHECE: 19:42");
    lv_obj_set_style_text_font(lbl_sunset, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(lbl_sunset, V3_TEXT_DARK, 0);
    lv_obj_set_pos(lbl_sunset, 257, 200);
}

// ============================================================================
// Forecast Panel - Row 3, Left
// Position: (15, 410), Size: (395, 135) - adjusted for fixed header/footer
// 3 DAY forecast with cards
// ============================================================================

void createForecastPanelV3(lv_obj_t *parent) {
    panel_forecast = createCardV3(parent, 15, 410, 395, 135);

    // "3 DAY" title
    lv_obj_t *title = lv_label_create(panel_forecast);
    lv_label_set_text_static(title, "3 DAY");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title, V3_TEXT_GRAY, 0);
    lv_obj_set_pos(title, 5, 2);

    const char *days[] = {"THU", "FRI", "SAT"};
    const char *temps[] = {"18/23°C", "17/22°C", "16/21°C"};

    for (int i = 0; i < 3; i++) {
        int x_off = 10 + i * 125;

        // Inner card
        forecast_cards[i] = lv_obj_create(panel_forecast);
        lv_obj_set_size(forecast_cards[i], 115, 100);
        lv_obj_set_pos(forecast_cards[i], x_off, 20);
        lv_obj_add_style(forecast_cards[i], &style_v3_card_inner, 0);
        lv_obj_clear_flag(forecast_cards[i], LV_OBJ_FLAG_SCROLLABLE);

        // Day name
        lbl_fc_day[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_day[i], days[i]);
        lv_obj_set_style_text_font(lbl_fc_day[i], &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(lbl_fc_day[i], V3_TEXT_DARK, 0);
        lv_obj_align(lbl_fc_day[i], LV_ALIGN_TOP_MID, 0, 5);

        // Weather icon
        img_fc_icon[i] = lv_img_create(forecast_cards[i]);
        lv_img_set_src(img_fc_icon[i], &img_sunny);
        lv_img_set_zoom(img_fc_icon[i], 384);  // 1.5x
        lv_obj_align(img_fc_icon[i], LV_ALIGN_CENTER, 0, 5);

        // Temperature (min/max)
        lbl_fc_temp[i] = lv_label_create(forecast_cards[i]);
        lv_label_set_text(lbl_fc_temp[i], temps[i]);
        lv_obj_set_style_text_font(lbl_fc_temp[i], &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(lbl_fc_temp[i], V3_TEXT_GRAY, 0);
        lv_obj_align(lbl_fc_temp[i], LV_ALIGN_BOTTOM_MID, 0, -5);
    }
}

// ============================================================================
// Fixed Header Bar - Top (on lv_layer_top, persists across screens)
// Position: (0, 0), Size: (1024, 40)
// Contains: WiFi status, Battery, Settings button
// ============================================================================

void createFixedHeader() {
    if (fixed_header) return;  // Already created

    fixed_header = lv_obj_create(lv_layer_top());
    lv_obj_set_pos(fixed_header, 0, 0);
    lv_obj_set_size(fixed_header, 1024, 40);
    lv_obj_set_style_bg_color(fixed_header, V3_CARD_COLOR, 0);
    lv_obj_set_style_bg_opa(fixed_header, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(fixed_header, 0, 0);
    lv_obj_set_style_border_width(fixed_header, 0, 0);
    lv_obj_set_style_shadow_width(fixed_header, 8, 0);
    lv_obj_set_style_shadow_ofs_y(fixed_header, 2, 0);
    lv_obj_set_style_shadow_opa(fixed_header, LV_OPA_10, 0);
    lv_obj_set_style_pad_hor(fixed_header, 15, 0);
    lv_obj_clear_flag(fixed_header, LV_OBJ_FLAG_SCROLLABLE);

    // WiFi icon
    lbl_wifi_icon = lv_label_create(fixed_header);
    lv_label_set_text(lbl_wifi_icon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(lbl_wifi_icon, V3_GREEN, 0);
    lv_obj_set_style_text_font(lbl_wifi_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_wifi_icon, LV_ALIGN_LEFT_MID, 10, 0);

    lbl_wifi_text = lv_label_create(fixed_header);
    lv_label_set_text(lbl_wifi_text, "CONECTANDO...");
    lv_obj_set_style_text_font(lbl_wifi_text, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_wifi_text, V3_TEXT_DARK, 0);
    lv_obj_align(lbl_wifi_text, LV_ALIGN_LEFT_MID, 35, 0);

    // Power source indicator (no battery on ESP32)
    lbl_battery_icon = lv_label_create(fixed_header);
    lv_label_set_text(lbl_battery_icon, LV_SYMBOL_CHARGE);
    lv_obj_set_style_text_color(lbl_battery_icon, V3_GREEN, 0);
    lv_obj_set_style_text_font(lbl_battery_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_battery_icon, LV_ALIGN_CENTER, 100, 0);

    lbl_battery_text = lv_label_create(fixed_header);
    lv_label_set_text(lbl_battery_text, "USB");
    lv_obj_set_style_text_font(lbl_battery_text, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_battery_text, V3_TEXT_DARK, 0);
    lv_obj_align(lbl_battery_text, LV_ALIGN_CENTER, 135, 0);

    // Data/API connection status
    lbl_data_icon = lv_label_create(fixed_header);
    lv_label_set_text(lbl_data_icon, LV_SYMBOL_DOWNLOAD);
    lv_obj_set_style_text_color(lbl_data_icon, V3_TEXT_GRAY, 0);
    lv_obj_set_style_text_font(lbl_data_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_data_icon, LV_ALIGN_RIGHT_MID, -180, 0);

    lbl_data_text = lv_label_create(fixed_header);
    lv_label_set_text(lbl_data_text, "DATOS: ---");
    lv_obj_set_style_text_font(lbl_data_text, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_data_text, V3_TEXT_GRAY, 0);
    lv_obj_align(lbl_data_text, LV_ALIGN_RIGHT_MID, -70, 0);

    // Settings button
    lv_obj_t *btn = lv_btn_create(fixed_header);
    lv_obj_set_size(btn, 32, 32);
    lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_set_style_bg_color(btn, V3_BG_COLOR, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        switchToTabsScreen();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_icon = lv_label_create(btn);
    lv_label_set_text(btn_icon, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(btn_icon, V3_TEXT_GRAY, 0);
    lv_obj_center(btn_icon);
}

// ============================================================================
// Fixed Footer Bar - Bottom (on lv_layer_top, persists across screens)
// Position: (0, 555), Size: (1024, 45)
// Dark background with UV, ICA, Visibility, Clouds
// ============================================================================

void createFixedFooter() {
    if (fixed_footer) return;  // Already created

    fixed_footer = lv_obj_create(lv_layer_top());
    lv_obj_set_pos(fixed_footer, 0, 555);
    lv_obj_set_size(fixed_footer, 1024, 45);
    lv_obj_set_style_bg_color(fixed_footer, V3_FOOTER_COLOR, 0);
    lv_obj_set_style_bg_opa(fixed_footer, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(fixed_footer, 0, 0);
    lv_obj_set_style_border_width(fixed_footer, 0, 0);
    lv_obj_set_style_pad_hor(fixed_footer, 20, 0);
    lv_obj_set_style_pad_ver(fixed_footer, 8, 0);
    lv_obj_clear_flag(fixed_footer, LV_OBJ_FLAG_SCROLLABLE);

    int spacing = 250;
    int y = 8;

    // UV Index with badge
    lv_obj_t *uv_badge = lv_label_create(fixed_footer);
    lv_label_set_text_static(uv_badge, "UV");
    lv_obj_set_style_text_color(uv_badge, lv_color_white(), 0);
    lv_obj_set_style_text_font(uv_badge, &lv_font_montserrat_12, 0);
    lv_obj_set_style_bg_color(uv_badge, V3_ORANGE, 0);
    lv_obj_set_style_bg_opa(uv_badge, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(uv_badge, 3, 0);
    lv_obj_set_style_radius(uv_badge, 4, 0);
    lv_obj_set_pos(uv_badge, 20, y);

    lbl_uv = lv_label_create(fixed_footer);
    lv_label_set_text(lbl_uv, "INDICE UV: BAJO (2)");
    lv_obj_set_style_text_font(lbl_uv, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_uv, lv_color_white(), 0);
    lv_obj_set_pos(lbl_uv, 50, y + 2);

    // ICA (Air Quality)
    lv_obj_t *ica_icon = lv_label_create(fixed_footer);
    lv_label_set_text(ica_icon, LV_SYMBOL_OK);
    lv_obj_set_style_text_color(ica_icon, V3_GREEN, 0);
    lv_obj_set_pos(ica_icon, spacing, y);

    lbl_ica = lv_label_create(fixed_footer);
    lv_label_set_text(lbl_ica, "ICA: 25");
    lv_obj_set_style_text_font(lbl_ica, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_ica, lv_color_white(), 0);
    lv_obj_set_pos(lbl_ica, spacing + 25, y + 2);

    // Visibility
    lv_obj_t *vis_icon = lv_label_create(fixed_footer);
    lv_label_set_text(vis_icon, LV_SYMBOL_EYE_OPEN);
    lv_obj_set_style_text_color(vis_icon, V3_TEXT_LIGHT, 0);
    lv_obj_set_pos(vis_icon, spacing * 2, y);

    lbl_visibility = lv_label_create(fixed_footer);
    lv_label_set_text(lbl_visibility, "VISIBILIDAD: 10 km");
    lv_obj_set_style_text_font(lbl_visibility, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_visibility, lv_color_white(), 0);
    lv_obj_set_pos(lbl_visibility, spacing * 2 + 25, y + 2);

    // Clouds
    lv_obj_t *cloud_icon = lv_label_create(fixed_footer);
    lv_label_set_text(cloud_icon, LV_SYMBOL_IMAGE);
    lv_obj_set_style_text_color(cloud_icon, V3_TEXT_LIGHT, 0);
    lv_obj_set_pos(cloud_icon, spacing * 3, y);

    lbl_clouds = lv_label_create(fixed_footer);
    lv_label_set_text(lbl_clouds, "NUBOSIDAD: 0%");
    lv_obj_set_style_text_font(lbl_clouds, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_clouds, lv_color_white(), 0);
    lv_obj_set_pos(lbl_clouds, spacing * 3 + 25, y + 2);
}

// ============================================================================
// Create Fixed Layers (Header + Footer)
// Call once at startup - they persist across all screens
// ============================================================================

void createFixedLayers() {
    if (fixed_layers_created) return;

    initStylesV3();
    createFixedHeader();
    createFixedFooter();

    fixed_layers_created = true;
    Serial.println("Fixed header/footer layers created on lv_layer_top()");
}

// ============================================================================
// Create Main Screen V3
// ============================================================================

/**
 * Create V3 content on a parent container (Hidden/Visible pattern)
 * Fixed layers (header/footer) are created separately on lv_layer_top
 */
void createMainScreenV3Content(lv_obj_t *parent) {
    Serial.println("Creating main screen V3 content...");

    initStylesV3();

    // Store reference for updates (points to parent container)
    main_screen_v3 = parent;

    // Apply background style to parent
    lv_obj_add_style(parent, &style_v3_bg, 0);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);

    // Create content panels (between fixed header and footer)
    createClockPanelV3(parent);
    createPressurePanelV3(parent);
    createTrendsPanelV3(parent);
    createTempPanelV3(parent);
    createHumidityPanelV3(parent);
    createWeatherPanelV3(parent);
    createForecastPanelV3(parent);

    Serial.println("Main screen V3 content created");
}

// Legacy function (creates separate screen - deprecated)
void createMainScreenV3() {
    Serial.println("Creating main screen V3 (legacy)...");

    initStylesV3();
    createFixedLayers();

    main_screen_v3 = lv_obj_create(NULL);
    lv_obj_add_style(main_screen_v3, &style_v3_bg, 0);
    lv_obj_clear_flag(main_screen_v3, LV_OBJ_FLAG_SCROLLABLE);

    createClockPanelV3(main_screen_v3);
    createPressurePanelV3(main_screen_v3);
    createTrendsPanelV3(main_screen_v3);
    createTempPanelV3(main_screen_v3);
    createHumidityPanelV3(main_screen_v3);
    createWeatherPanelV3(main_screen_v3);
    createForecastPanelV3(main_screen_v3);

    Serial.println("Main screen V3 created");
}

// ============================================================================
// Show/Hide Fixed Layers
// ============================================================================

void showFixedLayers() {
    if (fixed_header) lv_obj_clear_flag(fixed_header, LV_OBJ_FLAG_HIDDEN);
    if (fixed_footer) lv_obj_clear_flag(fixed_footer, LV_OBJ_FLAG_HIDDEN);
}

void hideFixedLayers() {
    if (fixed_header) lv_obj_add_flag(fixed_header, LV_OBJ_FLAG_HIDDEN);
    if (fixed_footer) lv_obj_add_flag(fixed_footer, LV_OBJ_FLAG_HIDDEN);
}

// ============================================================================
// Show/Get Functions
// ============================================================================

void showMainScreenV3() {
    if (main_screen_v3) {
        showFixedLayers();  // Show fixed header/footer
        lv_scr_load(main_screen_v3);
    }
}

lv_obj_t* getMainScreenV3() {
    return main_screen_v3;
}

// ============================================================================
// Update Functions
// ============================================================================

// Update WiFi signal strength in header
void updateWiFiStatus() {
    if (!lbl_wifi_text || !lbl_wifi_icon) return;

    if (WiFi.status() == WL_CONNECTED) {
        int rssi = WiFi.RSSI();
        char buf[32];
        snprintf(buf, sizeof(buf), "%s (%ddBm)", getWiFiStrengthText(rssi), rssi);
        lv_label_set_text(lbl_wifi_text, buf);

        lv_color_t color = getWiFiStrengthColor(rssi);
        lv_obj_set_style_text_color(lbl_wifi_icon, color, 0);
    } else {
        lv_label_set_text(lbl_wifi_text, "DESCONECTADO");
        lv_obj_set_style_text_color(lbl_wifi_icon, lv_color_hex(0xF44336), 0);
    }
}

void updateMainScreenV3Time() {
    if (!lbl_clock_time) return;

    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);

    char buf[20];
    snprintf(buf, sizeof(buf), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    lv_label_set_text(lbl_clock_time, buf);

    const char *months[] = {"ENE", "FEB", "MAR", "ABR", "MAY", "JUN",
                            "JUL", "AGO", "SEP", "OCT", "NOV", "DIC"};
    snprintf(buf, sizeof(buf), "%02d %s %d",
        timeinfo.tm_mday, months[timeinfo.tm_mon], timeinfo.tm_year + 1900);
    lv_label_set_text(lbl_clock_date, buf);
}

void updateMainScreenV3Weather(WeatherData *weather) {
    if (!weather || !lbl_temp_value) return;

    char buf[64];

    // Add sample to trend history
    updateTrendHistory(weather->temp_c, weather->pressure_mb, weather->humidity);

    // Temperature
    snprintf(buf, sizeof(buf), "%.1f°C", weather->temp_c);
    lv_label_set_text(lbl_temp_value, buf);

    // Humidity
    snprintf(buf, sizeof(buf), "%d%%", weather->humidity);
    lv_label_set_text(lbl_hum_value, buf);
    if (arc_hum_indicator) {
        lv_arc_set_value(arc_hum_indicator, weather->humidity);
    }

    // Pressure
    snprintf(buf, sizeof(buf), "%.1fmb", weather->pressure_mb);
    lv_label_set_text(lbl_pressure_value, buf);

    // Update trend labels and arrows
    if (trendManager.hasEnoughData()) {
        TrendResult pressTrend = trendManager.getPressureTrend();
        TrendResult tempTrend = trendManager.getTemperatureTrend();
        TrendResult humTrend = trendManager.getHumidityTrend();

        // Pressure trend
        if (lbl_trend_press) {
            char trendStr[32];
            formatTrendString(trendStr, sizeof(trendStr), pressTrend, "mb");
            snprintf(buf, sizeof(buf), "TEND. PRESION: %s", trendStr);
            lv_label_set_text(lbl_trend_press, buf);
        }
        if (lbl_trend_press_arrow) {
            lv_label_set_text(lbl_trend_press_arrow, getTrendArrowLVGL(pressTrend.direction));
            lv_obj_set_style_text_color(lbl_trend_press_arrow,
                lv_color_hex(getTrendColor(pressTrend.direction)), 0);
        }

        // Temperature trend
        if (lbl_trend_temp) {
            char trendStr[32];
            formatTrendString(trendStr, sizeof(trendStr), tempTrend, "°C");
            snprintf(buf, sizeof(buf), "TEND. TEMP: %s", trendStr);
            lv_label_set_text(lbl_trend_temp, buf);
        }
        if (lbl_trend_temp_arrow) {
            lv_label_set_text(lbl_trend_temp_arrow, getTrendArrowLVGL(tempTrend.direction));
            lv_obj_set_style_text_color(lbl_trend_temp_arrow,
                lv_color_hex(getTrendColor(tempTrend.direction)), 0);
        }
        if (lbl_trend_temp_arrow2) {
            // Diagonal arrow visibility based on rising trend
            if (tempTrend.direction == TREND_RISING) {
                lv_obj_clear_flag(lbl_trend_temp_arrow2, LV_OBJ_FLAG_HIDDEN);
            } else {
                lv_obj_add_flag(lbl_trend_temp_arrow2, LV_OBJ_FLAG_HIDDEN);
            }
        }

        // Humidity trend
        if (lbl_trend_hum) {
            char trendStr[32];
            formatTrendString(trendStr, sizeof(trendStr), humTrend, "%");
            snprintf(buf, sizeof(buf), "TEND. HUMEDAD: %s", trendStr);
            lv_label_set_text(lbl_trend_hum, buf);
        }
    }

    // Sky condition (current)
    snprintf(buf, sizeof(buf), "CIELO: %s", weather->condition_text);
    for (int i = 0; buf[i]; i++) buf[i] = toupper(buf[i]);
    lv_label_set_text(lbl_sky_condition, buf);

    // Night description will be updated by updateMainScreenV3Forecast with real mintemp

    // Update WiFi status
    updateWiFiStatus();

    // Sunrise/Sunset
    snprintf(buf, sizeof(buf), "AMANECE: %s", weather->sunrise);
    lv_label_set_text(lbl_sunrise, buf);
    snprintf(buf, sizeof(buf), "ANOCHECE: %s", weather->sunset);
    lv_label_set_text(lbl_sunset, buf);

    // UV
    const char *uv_level;
    if (weather->uv < 3) uv_level = "BAJO";
    else if (weather->uv < 6) uv_level = "MODERADO";
    else if (weather->uv < 8) uv_level = "ALTO";
    else if (weather->uv < 11) uv_level = "MUY ALTO";
    else uv_level = "EXTREMO";
    snprintf(buf, sizeof(buf), "INDICE UV: %s (%.0f)", uv_level, weather->uv);
    lv_label_set_text(lbl_uv, buf);

    // ICA
    snprintf(buf, sizeof(buf), "ICA: %.0f", weather->aqi_pm25);
    lv_label_set_text(lbl_ica, buf);

    // Visibility
    snprintf(buf, sizeof(buf), "VISIBILIDAD: %.0f km", weather->vis_km);
    lv_label_set_text(lbl_visibility, buf);

    // Clouds
    snprintf(buf, sizeof(buf), "NUBOSIDAD: %d%%", weather->cloud);
    lv_label_set_text(lbl_clouds, buf);

    // Weather icon
    if (img_weather_icon) {
        lv_img_set_src(img_weather_icon, getWeatherIconBitmap(weather->condition_code, weather->is_day));
    }
}

void updateMainScreenV3Forecast(ForecastData *forecast) {
    if (!forecast) return;

    const char *day_names[] = {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"};
    char buf[48];

    // Update night description with real forecast data (today's min temp)
    if (lbl_sky_night) {
        generateNightDescription(buf, sizeof(buf),
            forecast[0].mintemp_c,
            (int)forecast[0].avghumidity,
            forecast[0].daily_chance_of_rain > 50 ? 80 : 20);  // Estimate clouds from rain chance
        lv_label_set_text(lbl_sky_night, buf);
    }

    for (int i = 0; i < 3 && i < MAX_FORECAST_DAYS; i++) {
        if (lbl_fc_day[i]) {
            if (i == 0) {
                lv_label_set_text(lbl_fc_day[i], "HOY");
            } else {
                struct tm timeinfo;
                time_t t = forecast[i].date_epoch;
                localtime_r(&t, &timeinfo);
                lv_label_set_text(lbl_fc_day[i], day_names[timeinfo.tm_wday]);
            }
        }

        if (lbl_fc_temp[i]) {
            snprintf(buf, sizeof(buf), "%.0f/%.0f°C",
                forecast[i].mintemp_c, forecast[i].maxtemp_c);
            lv_label_set_text(lbl_fc_temp[i], buf);
        }

        if (img_fc_icon[i]) {
            lv_img_set_src(img_fc_icon[i],
                getWeatherIconBitmap(forecast[i].condition_code, true));
        }
    }
}

// ============================================================================
// Connection Status Update (Watchdog)
// ============================================================================

void updateConnectionStatusV3() {
    if (!lbl_data_icon || !lbl_data_text) return;

    DataStatus status = dataWatchdog.getStatus();
    const char* icon;
    const char* text;
    lv_color_t color;

    switch (status) {
        case DATA_STATUS_OK:
            icon = LV_SYMBOL_OK;
            text = "DATOS: OK";
            color = V3_GREEN;
            break;

        case DATA_STATUS_STALE:
            icon = LV_SYMBOL_WARNING;
            text = "DATOS: ANTIGUOS";
            color = V3_ORANGE;
            break;

        case DATA_STATUS_DISCONNECTED:
            icon = LV_SYMBOL_CLOSE;
            text = "SIN CONEXION API";
            color = V3_RED;
            break;

        case DATA_STATUS_NO_WIFI:
            icon = LV_SYMBOL_WARNING;
            text = "SIN WIFI";
            color = V3_TEXT_GRAY;
            break;

        default:
            icon = LV_SYMBOL_REFRESH;
            text = "DATOS: ---";
            color = V3_TEXT_GRAY;
            break;
    }

    lv_label_set_text(lbl_data_icon, icon);
    lv_obj_set_style_text_color(lbl_data_icon, color, 0);

    lv_label_set_text(lbl_data_text, text);
    lv_obj_set_style_text_color(lbl_data_text, color, 0);
}

#endif // UI_MAIN_SCREEN_V3_H
