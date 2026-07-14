/**
 * ui_navigation.h - Sistema de navegación entre pantallas
 *
 * Gestos soportados:
 * - Swipe izquierda/derecha: cambiar pantalla
 * - Tap en panel: abrir modal
 * - Long press: cambiar unidades
 */

#ifndef UI_NAVIGATION_H
#define UI_NAVIGATION_H

#include <Arduino.h>
#include <lvgl.h>
#include "config.h"

// Forward declarations
void createHistoryScreen();
void createSettingsScreen();

// ============================================================================
// Pantallas disponibles
// ============================================================================

typedef enum {
    SCREEN_DASHBOARD = 0,
    SCREEN_HISTORY,
    SCREEN_SETTINGS,
    SCREEN_COUNT
} ScreenType;

static ScreenType currentScreen = SCREEN_DASHBOARD;

// Referencias a pantallas (definidas en sus respectivos headers)
extern lv_obj_t *scr_dashboard;
static lv_obj_t *scr_history = nullptr;
static lv_obj_t *scr_settings = nullptr;

// ============================================================================
// Variables de gestos
// ============================================================================

static lv_point_t gesture_start;
static bool gesture_active = false;
static uint32_t gesture_start_time = 0;

#define SWIPE_THRESHOLD     80      // Píxeles mínimos para swipe
#define SWIPE_TIME_MAX      500     // Tiempo máximo para swipe (ms)
#define LONG_PRESS_TIME     800     // Tiempo para long press (ms)

// ============================================================================
// Animaciones
// ============================================================================

#define SCREEN_ANIM_TIME    300     // Duración de transición (ms)

static void animScreenIn(lv_obj_t *screen, bool fromRight) {
    lv_obj_set_x(screen, fromRight ? SCREEN_WIDTH : -SCREEN_WIDTH);
    lv_scr_load(screen);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, screen);
    lv_anim_set_values(&a, fromRight ? SCREEN_WIDTH : -SCREEN_WIDTH, 0);
    lv_anim_set_time(&a, SCREEN_ANIM_TIME);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

// ============================================================================
// Navegación entre pantallas
// ============================================================================

void navigateToScreen(ScreenType screen) {
    if (screen == currentScreen || screen >= SCREEN_COUNT) return;

    bool goingRight = (screen > currentScreen);
    lv_obj_t *target = nullptr;

    switch (screen) {
        case SCREEN_DASHBOARD:
            target = scr_dashboard;
            break;
        case SCREEN_HISTORY:
            if (!scr_history) createHistoryScreen();
            target = scr_history;
            break;
        case SCREEN_SETTINGS:
            if (!scr_settings) createSettingsScreen();
            target = scr_settings;
            break;
        default:
            return;
    }

    if (target) {
        animScreenIn(target, goingRight);
        currentScreen = screen;
        Serial.printf("[NAV] Pantalla: %d\n", screen);
    }
}

void navigateNext() {
    if (currentScreen < SCREEN_COUNT - 1) {
        navigateToScreen((ScreenType)(currentScreen + 1));
    }
}

void navigatePrev() {
    if (currentScreen > 0) {
        navigateToScreen((ScreenType)(currentScreen - 1));
    }
}

// ============================================================================
// Indicador de página (dots)
// ============================================================================

static lv_obj_t *page_indicator = nullptr;
static lv_obj_t *page_dots[SCREEN_COUNT] = {nullptr};

void createPageIndicator(lv_obj_t *parent) {
    page_indicator = lv_obj_create(parent);
    lv_obj_set_size(page_indicator, 80, 20);
    lv_obj_align(page_indicator, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_obj_set_style_bg_opa(page_indicator, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(page_indicator, 0, 0);
    lv_obj_clear_flag(page_indicator, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(page_indicator, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(page_indicator, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(page_indicator, 8, 0);

    for (int i = 0; i < SCREEN_COUNT; i++) {
        page_dots[i] = lv_obj_create(page_indicator);
        lv_obj_set_size(page_dots[i], 8, 8);
        lv_obj_set_style_radius(page_dots[i], LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(page_dots[i],
            i == 0 ? lv_color_hex(0x2196F3) : lv_color_hex(0xBDBDBD), 0);
        lv_obj_set_style_border_width(page_dots[i], 0, 0);
    }
}

void updatePageIndicator() {
    for (int i = 0; i < SCREEN_COUNT; i++) {
        if (page_dots[i]) {
            lv_obj_set_style_bg_color(page_dots[i],
                i == currentScreen ? lv_color_hex(0x2196F3) : lv_color_hex(0xBDBDBD), 0);
        }
    }
}

// ============================================================================
// Manejador de gestos (swipe)
// ============================================================================

static void gestureEventHandler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_PRESSED) {
        lv_indev_t *indev = lv_indev_get_act();
        if (indev) {
            lv_indev_get_point(indev, &gesture_start);
            gesture_active = true;
            gesture_start_time = lv_tick_get();
        }
    }
    else if (code == LV_EVENT_RELEASED && gesture_active) {
        lv_indev_t *indev = lv_indev_get_act();
        if (indev) {
            lv_point_t gesture_end;
            lv_indev_get_point(indev, &gesture_end);

            int32_t dx = gesture_end.x - gesture_start.x;
            int32_t dy = gesture_end.y - gesture_start.y;
            uint32_t elapsed = lv_tick_get() - gesture_start_time;

            // Verificar si es swipe horizontal
            if (elapsed < SWIPE_TIME_MAX && abs(dx) > SWIPE_THRESHOLD && abs(dx) > abs(dy) * 2) {
                if (dx < 0) {
                    navigateNext();  // Swipe izquierda → siguiente
                } else {
                    navigatePrev();  // Swipe derecha → anterior
                }
                updatePageIndicator();
            }
        }
        gesture_active = false;
    }
}

void enableSwipeNavigation(lv_obj_t *screen) {
    lv_obj_add_event_cb(screen, gestureEventHandler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(screen, gestureEventHandler, LV_EVENT_RELEASED, NULL);
}

// ============================================================================
// Pantalla de Historial (gráficas)
// ============================================================================

static lv_obj_t *chart_temp = nullptr;
static lv_obj_t *chart_humidity = nullptr;
static lv_obj_t *chart_pressure = nullptr;
static lv_chart_series_t *ser_temp = nullptr;
static lv_chart_series_t *ser_humidity = nullptr;
static lv_chart_series_t *ser_pressure = nullptr;

void createHistoryScreen() {
    extern lv_style_t style_card;

    scr_history = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_history, lv_color_hex(0xF5F5F5), 0);
    lv_obj_clear_flag(scr_history, LV_OBJ_FLAG_SCROLLABLE);

    // Título
    lv_obj_t *title = lv_label_create(scr_history);
    lv_label_set_text(title, "HISTORIAL 24 HORAS");
    lv_obj_set_style_text_color(title, lv_color_hex(0x2C2C2C), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    // Botón volver
    lv_obj_t *btn_back = lv_btn_create(scr_history);
    lv_obj_set_size(btn_back, 80, 40);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_set_style_bg_color(btn_back, lv_color_hex(0xE0E0E0), 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        navigateToScreen(SCREEN_DASHBOARD);
        updatePageIndicator();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn_back);
    lv_label_set_text(btn_label, LV_SYMBOL_LEFT " Volver");
    lv_obj_center(btn_label);

    // Gráfica de temperatura
    lv_obj_t *card_temp = lv_obj_create(scr_history);
    lv_obj_set_size(card_temp, 480, 200);
    lv_obj_set_pos(card_temp, 15, 60);
    lv_obj_add_style(card_temp, &style_card, 0);
    lv_obj_clear_flag(card_temp, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_temp = lv_label_create(card_temp);
    lv_label_set_text(lbl_temp, "🌡️ TEMPERATURA (°C)");
    lv_obj_set_style_text_font(lbl_temp, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_temp, LV_ALIGN_TOP_LEFT, 5, 5);

    chart_temp = lv_chart_create(card_temp);
    lv_obj_set_size(chart_temp, 440, 140);
    lv_obj_align(chart_temp, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_chart_set_type(chart_temp, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_temp, 24);
    lv_chart_set_range(chart_temp, LV_CHART_AXIS_PRIMARY_Y, 10, 40);
    lv_obj_set_style_line_width(chart_temp, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(chart_temp, 4, LV_PART_INDICATOR);
    ser_temp = lv_chart_add_series(chart_temp, lv_color_hex(0xFF6B6B), LV_CHART_AXIS_PRIMARY_Y);

    // Gráfica de humedad
    lv_obj_t *card_hum = lv_obj_create(scr_history);
    lv_obj_set_size(card_hum, 480, 200);
    lv_obj_set_pos(card_hum, 510, 60);
    lv_obj_add_style(card_hum, &style_card, 0);
    lv_obj_clear_flag(card_hum, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_hum = lv_label_create(card_hum);
    lv_label_set_text(lbl_hum, "💧 HUMEDAD (%)");
    lv_obj_set_style_text_font(lbl_hum, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_hum, LV_ALIGN_TOP_LEFT, 5, 5);

    chart_humidity = lv_chart_create(card_hum);
    lv_obj_set_size(chart_humidity, 440, 140);
    lv_obj_align(chart_humidity, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_chart_set_type(chart_humidity, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_humidity, 24);
    lv_chart_set_range(chart_humidity, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_obj_set_style_line_width(chart_humidity, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(chart_humidity, 4, LV_PART_INDICATOR);
    ser_humidity = lv_chart_add_series(chart_humidity, lv_color_hex(0x2196F3), LV_CHART_AXIS_PRIMARY_Y);

    // Gráfica de presión
    lv_obj_t *card_pres = lv_obj_create(scr_history);
    lv_obj_set_size(card_pres, 480, 200);
    lv_obj_set_pos(card_pres, 15, 275);
    lv_obj_add_style(card_pres, &style_card, 0);
    lv_obj_clear_flag(card_pres, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_pres = lv_label_create(card_pres);
    lv_label_set_text(lbl_pres, "📊 PRESIÓN (hPa)");
    lv_obj_set_style_text_font(lbl_pres, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_pres, LV_ALIGN_TOP_LEFT, 5, 5);

    chart_pressure = lv_chart_create(card_pres);
    lv_obj_set_size(chart_pressure, 440, 140);
    lv_obj_align(chart_pressure, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_chart_set_type(chart_pressure, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_pressure, 24);
    lv_chart_set_range(chart_pressure, LV_CHART_AXIS_PRIMARY_Y, 990, 1040);
    lv_obj_set_style_line_width(chart_pressure, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(chart_pressure, 4, LV_PART_INDICATOR);
    ser_pressure = lv_chart_add_series(chart_pressure, lv_color_hex(0x4CAF50), LV_CHART_AXIS_PRIMARY_Y);

    // Panel de estadísticas
    lv_obj_t *card_stats = lv_obj_create(scr_history);
    lv_obj_set_size(card_stats, 480, 200);
    lv_obj_set_pos(card_stats, 510, 275);
    lv_obj_add_style(card_stats, &style_card, 0);
    lv_obj_clear_flag(card_stats, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_stats = lv_label_create(card_stats);
    lv_label_set_text(lbl_stats, "📈 ESTADÍSTICAS DEL DÍA");
    lv_obj_set_style_text_font(lbl_stats, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_stats, LV_ALIGN_TOP_LEFT, 5, 5);

    // Rellenar datos de ejemplo
    for (int i = 0; i < 24; i++) {
        lv_chart_set_next_value(chart_temp, ser_temp, 18 + (rand() % 10));
        lv_chart_set_next_value(chart_humidity, ser_humidity, 50 + (rand() % 30));
        lv_chart_set_next_value(chart_pressure, ser_pressure, 1010 + (rand() % 15));
    }

    // Habilitar swipe
    enableSwipeNavigation(scr_history);

    Serial.println("[UI] Pantalla historial creada");
}

// ============================================================================
// Pantalla de Configuración
// ============================================================================

static lv_obj_t *sw_celsius = nullptr;
static lv_obj_t *sw_24h = nullptr;
static lv_obj_t *slider_brightness = nullptr;

void createSettingsScreen() {
    extern lv_style_t style_card;

    scr_settings = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_settings, lv_color_hex(0xF5F5F5), 0);
    lv_obj_clear_flag(scr_settings, LV_OBJ_FLAG_SCROLLABLE);

    // Título
    lv_obj_t *title = lv_label_create(scr_settings);
    lv_label_set_text(title, LV_SYMBOL_SETTINGS " CONFIGURACIÓN");
    lv_obj_set_style_text_color(title, lv_color_hex(0x2C2C2C), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

    // Botón volver
    lv_obj_t *btn_back = lv_btn_create(scr_settings);
    lv_obj_set_size(btn_back, 80, 40);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_set_style_bg_color(btn_back, lv_color_hex(0xE0E0E0), 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        navigateToScreen(SCREEN_DASHBOARD);
        updatePageIndicator();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn_back);
    lv_label_set_text(btn_label, LV_SYMBOL_LEFT " Volver");
    lv_obj_center(btn_label);

    // Card: Unidades
    lv_obj_t *card_units = lv_obj_create(scr_settings);
    lv_obj_set_size(card_units, 480, 150);
    lv_obj_set_pos(card_units, 15, 70);
    lv_obj_add_style(card_units, &style_card, 0);
    lv_obj_clear_flag(card_units, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_units = lv_label_create(card_units);
    lv_label_set_text(lbl_units, "UNIDADES");
    lv_obj_set_style_text_color(lbl_units, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(lbl_units, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_units, LV_ALIGN_TOP_LEFT, 0, 0);

    // Temperatura
    lv_obj_t *lbl_temp = lv_label_create(card_units);
    lv_label_set_text(lbl_temp, "Temperatura:");
    lv_obj_set_style_text_font(lbl_temp, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_temp, LV_ALIGN_TOP_LEFT, 0, 30);

    sw_celsius = lv_switch_create(card_units);
    lv_obj_align(sw_celsius, LV_ALIGN_TOP_LEFT, 200, 25);
    lv_obj_add_state(sw_celsius, LV_STATE_CHECKED);

    lv_obj_t *lbl_f = lv_label_create(card_units);
    lv_label_set_text(lbl_f, "°F");
    lv_obj_align_to(lbl_f, sw_celsius, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    lv_obj_t *lbl_c = lv_label_create(card_units);
    lv_label_set_text(lbl_c, "°C");
    lv_obj_align_to(lbl_c, sw_celsius, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Formato hora
    lv_obj_t *lbl_time = lv_label_create(card_units);
    lv_label_set_text(lbl_time, "Formato hora:");
    lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_time, LV_ALIGN_TOP_LEFT, 0, 70);

    sw_24h = lv_switch_create(card_units);
    lv_obj_align(sw_24h, LV_ALIGN_TOP_LEFT, 200, 65);
    lv_obj_add_state(sw_24h, LV_STATE_CHECKED);

    lv_obj_t *lbl_12h = lv_label_create(card_units);
    lv_label_set_text(lbl_12h, "12h");
    lv_obj_align_to(lbl_12h, sw_24h, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    lv_obj_t *lbl_24h = lv_label_create(card_units);
    lv_label_set_text(lbl_24h, "24h");
    lv_obj_align_to(lbl_24h, sw_24h, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Card: Display
    lv_obj_t *card_display = lv_obj_create(scr_settings);
    lv_obj_set_size(card_display, 480, 120);
    lv_obj_set_pos(card_display, 15, 235);
    lv_obj_add_style(card_display, &style_card, 0);
    lv_obj_clear_flag(card_display, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_display = lv_label_create(card_display);
    lv_label_set_text(lbl_display, "PANTALLA");
    lv_obj_set_style_text_color(lbl_display, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(lbl_display, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_display, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *lbl_bright = lv_label_create(card_display);
    lv_label_set_text(lbl_bright, "Brillo:");
    lv_obj_set_style_text_font(lbl_bright, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_bright, LV_ALIGN_TOP_LEFT, 0, 35);

    slider_brightness = lv_slider_create(card_display);
    lv_obj_set_width(slider_brightness, 300);
    lv_obj_align(slider_brightness, LV_ALIGN_TOP_LEFT, 100, 40);
    lv_slider_set_range(slider_brightness, 10, 255);
    lv_slider_set_value(slider_brightness, 200, LV_ANIM_OFF);

    lv_obj_t *lbl_bright_val = lv_label_create(card_display);
    lv_label_set_text(lbl_bright_val, "78%");
    lv_obj_align_to(lbl_bright_val, slider_brightness, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

    lv_obj_add_event_cb(slider_brightness, [](lv_event_t *e) {
        lv_obj_t *slider = lv_event_get_target(e);
        int val = lv_slider_get_value(slider);
        // Actualizar PWM del backlight
        // analogWrite(LCD_BACKLIGHT, val);

        // Actualizar label
        lv_obj_t *parent = lv_obj_get_parent(slider);
        lv_obj_t *label = lv_obj_get_child(parent, -1);
        if (label) {
            char buf[10];
            snprintf(buf, sizeof(buf), "%d%%", val * 100 / 255);
            lv_label_set_text(label, buf);
        }
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Card: Conexión
    lv_obj_t *card_conn = lv_obj_create(scr_settings);
    lv_obj_set_size(card_conn, 480, 180);
    lv_obj_set_pos(card_conn, 510, 70);
    lv_obj_add_style(card_conn, &style_card, 0);
    lv_obj_clear_flag(card_conn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_conn = lv_label_create(card_conn);
    lv_label_set_text(lbl_conn, "CONEXIÓN");
    lv_obj_set_style_text_color(lbl_conn, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(lbl_conn, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_conn, LV_ALIGN_TOP_LEFT, 0, 0);

    extern SystemStatus g_status;

    lv_obj_t *lbl_wifi = lv_label_create(card_conn);
    char wifi_txt[64];
    snprintf(wifi_txt, sizeof(wifi_txt), "WiFi: %s (%d dBm)",
             g_status.wifi_connected ? "Conectado" : "Desconectado",
             g_status.wifi_rssi);
    lv_label_set_text(lbl_wifi, wifi_txt);
    lv_obj_set_style_text_font(lbl_wifi, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_wifi, LV_ALIGN_TOP_LEFT, 0, 30);

    lv_obj_t *lbl_api = lv_label_create(card_conn);
    lv_label_set_text(lbl_api, g_status.api_ok ?
        "🟢 Servidor: Conectado" : "🔴 Servidor: Sin conexión");
    lv_obj_set_style_text_font(lbl_api, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_api, LV_ALIGN_TOP_LEFT, 0, 60);

    lv_obj_t *lbl_bme = lv_label_create(card_conn);
    lv_label_set_text(lbl_bme, g_status.bme280_ok ?
        "🟢 BME280: OK" : "⚪ BME280: No detectado");
    lv_obj_set_style_text_font(lbl_bme, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_bme, LV_ALIGN_TOP_LEFT, 0, 90);

    // Botón reconfigurar WiFi
    lv_obj_t *btn_wifi = lv_btn_create(card_conn);
    lv_obj_set_size(btn_wifi, 180, 40);
    lv_obj_align(btn_wifi, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_color(btn_wifi, lv_color_hex(0x2196F3), 0);

    lv_obj_t *lbl_btn_wifi = lv_label_create(btn_wifi);
    lv_label_set_text(lbl_btn_wifi, LV_SYMBOL_WIFI " Reconfigurar");
    lv_obj_set_style_text_color(lbl_btn_wifi, lv_color_white(), 0);
    lv_obj_center(lbl_btn_wifi);

    // Card: Sistema
    lv_obj_t *card_sys = lv_obj_create(scr_settings);
    lv_obj_set_size(card_sys, 480, 175);
    lv_obj_set_pos(card_sys, 510, 265);
    lv_obj_add_style(card_sys, &style_card, 0);
    lv_obj_clear_flag(card_sys, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_sys = lv_label_create(card_sys);
    lv_label_set_text(lbl_sys, "SISTEMA");
    lv_obj_set_style_text_color(lbl_sys, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(lbl_sys, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_sys, LV_ALIGN_TOP_LEFT, 0, 0);

    char sys_txt[128];
    snprintf(sys_txt, sizeof(sys_txt),
        "Uptime: %lu min\n"
        "Heap libre: %.1f KB\n"
        "PSRAM libre: %.1f MB\n"
        "Versión: 1.0.0",
        millis() / 60000,
        ESP.getFreeHeap() / 1024.0,
        ESP.getFreePsram() / 1024.0 / 1024.0);

    lv_obj_t *lbl_sys_info = lv_label_create(card_sys);
    lv_label_set_text(lbl_sys_info, sys_txt);
    lv_obj_set_style_text_font(lbl_sys_info, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_sys_info, LV_ALIGN_TOP_LEFT, 0, 30);

    // Botón reiniciar
    lv_obj_t *btn_restart = lv_btn_create(card_sys);
    lv_obj_set_size(btn_restart, 120, 40);
    lv_obj_align(btn_restart, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(btn_restart, lv_color_hex(0xF44336), 0);
    lv_obj_add_event_cb(btn_restart, [](lv_event_t *e) {
        ESP.restart();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_restart = lv_label_create(btn_restart);
    lv_label_set_text(lbl_restart, LV_SYMBOL_REFRESH " Reiniciar");
    lv_obj_set_style_text_color(lbl_restart, lv_color_white(), 0);
    lv_obj_center(lbl_restart);

    // Habilitar swipe
    enableSwipeNavigation(scr_settings);

    Serial.println("[UI] Pantalla configuración creada");
}

// ============================================================================
// Modales (tap para expandir)
// ============================================================================

static lv_obj_t *modal_overlay = nullptr;
static lv_obj_t *modal_content = nullptr;

void closeModal(lv_event_t *e) {
    if (modal_overlay) {
        lv_obj_del(modal_overlay);
        modal_overlay = nullptr;
        modal_content = nullptr;
    }
}

void showModal(const char *title, lv_obj_t *content_creator(lv_obj_t *parent)) {
    extern lv_obj_t *scr_dashboard;

    // Overlay semi-transparente
    modal_overlay = lv_obj_create(lv_scr_act());
    lv_obj_set_size(modal_overlay, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_pos(modal_overlay, 0, 0);
    lv_obj_set_style_bg_color(modal_overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(modal_overlay, LV_OPA_50, 0);
    lv_obj_set_style_border_width(modal_overlay, 0, 0);
    lv_obj_add_event_cb(modal_overlay, closeModal, LV_EVENT_CLICKED, NULL);

    // Contenedor del modal
    modal_content = lv_obj_create(modal_overlay);
    lv_obj_set_size(modal_content, 700, 450);
    lv_obj_center(modal_content);
    lv_obj_set_style_bg_color(modal_content, lv_color_white(), 0);
    lv_obj_set_style_radius(modal_content, 20, 0);
    lv_obj_set_style_shadow_width(modal_content, 30, 0);
    lv_obj_set_style_shadow_opa(modal_content, LV_OPA_30, 0);
    lv_obj_set_style_pad_all(modal_content, 20, 0);
    lv_obj_clear_flag(modal_content, LV_OBJ_FLAG_SCROLLABLE);

    // Evitar que clicks en el modal cierren
    lv_obj_add_flag(modal_content, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(modal_content, [](lv_event_t *e) {
        lv_event_stop_bubbling(e);
    }, LV_EVENT_CLICKED, NULL);

    // Título
    lv_obj_t *lbl_title = lv_label_create(modal_content);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(lbl_title, lv_color_hex(0x2C2C2C), 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 0);

    // Botón cerrar
    lv_obj_t *btn_close = lv_btn_create(modal_content);
    lv_obj_set_size(btn_close, 40, 40);
    lv_obj_align(btn_close, LV_ALIGN_TOP_RIGHT, 0, -5);
    lv_obj_set_style_bg_color(btn_close, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_radius(btn_close, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(btn_close, closeModal, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_x = lv_label_create(btn_close);
    lv_label_set_text(lbl_x, LV_SYMBOL_CLOSE);
    lv_obj_center(lbl_x);

    // Contenido personalizado
    if (content_creator) {
        content_creator(modal_content);
    }
}

// Modal: Detalle de temperatura
lv_obj_t* modalTemperatureContent(lv_obj_t *parent) {
    extern WeatherData g_weather;
    extern CompareData g_compare;

    char buf[64];

    // Temperatura actual grande
    lv_obj_t *lbl_temp = lv_label_create(parent);
    snprintf(buf, sizeof(buf), "%.1f°C", g_weather.temp_outdoor);
    lv_label_set_text(lbl_temp, buf);
    lv_obj_set_style_text_font(lbl_temp, &lv_font_montserrat_48, 0);
    lv_obj_align(lbl_temp, LV_ALIGN_TOP_LEFT, 20, 50);

    // Sensación térmica
    lv_obj_t *lbl_feels = lv_label_create(parent);
    snprintf(buf, sizeof(buf), "Sensación térmica: %.1f°C", g_weather.feels_like);
    lv_label_set_text(lbl_feels, buf);
    lv_obj_set_style_text_font(lbl_feels, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_feels, LV_ALIGN_TOP_LEFT, 20, 120);

    // Punto de rocío
    lv_obj_t *lbl_dew = lv_label_create(parent);
    snprintf(buf, sizeof(buf), "Punto de rocío: %.1f°C", g_weather.dewpoint);
    lv_label_set_text(lbl_dew, buf);
    lv_obj_set_style_text_font(lbl_dew, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_dew, LV_ALIGN_TOP_LEFT, 20, 150);

    // Máxima/Mínima
    lv_obj_t *lbl_range = lv_label_create(parent);
    snprintf(buf, sizeof(buf), "Hoy: Máx %.1f°C  /  Mín %.1f°C",
             g_weather.temp_max, g_weather.temp_min);
    lv_label_set_text(lbl_range, buf);
    lv_obj_set_style_text_font(lbl_range, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_range, LV_ALIGN_TOP_LEFT, 20, 190);

    // Comparación
    if (g_compare.valid) {
        lv_obj_t *lbl_comp = lv_label_create(parent);
        snprintf(buf, sizeof(buf), "vs ayer: %+.1f°C", g_compare.temp_diff);
        lv_label_set_text(lbl_comp, buf);
        lv_obj_set_style_text_font(lbl_comp, &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(lbl_comp,
            g_compare.temp_diff > 0 ? lv_color_hex(0xFF6B6B) : lv_color_hex(0x64B5F6), 0);
        lv_obj_align(lbl_comp, LV_ALIGN_TOP_LEFT, 20, 230);
    }

    // Gráfica mini 24h
    lv_obj_t *chart = lv_chart_create(parent);
    lv_obj_set_size(chart, 350, 150);
    lv_obj_align(chart, LV_ALIGN_RIGHT_MID, -20, 30);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 24);
    lv_obj_set_style_line_width(chart, 2, LV_PART_ITEMS);

    lv_chart_series_t *ser = lv_chart_add_series(chart, lv_color_hex(0xFF6B6B), LV_CHART_AXIS_PRIMARY_Y);
    for (int i = 0; i < 24; i++) {
        lv_chart_set_next_value(chart, ser, 18 + (rand() % 10));
    }

    lv_obj_t *lbl_chart = lv_label_create(parent);
    lv_label_set_text(lbl_chart, "Últimas 24 horas");
    lv_obj_set_style_text_font(lbl_chart, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(lbl_chart, lv_color_hex(0x999999), 0);
    lv_obj_align(lbl_chart, LV_ALIGN_BOTTOM_RIGHT, -120, -10);

    return parent;
}

void showTemperatureModal() {
    showModal("🌡️ TEMPERATURA", modalTemperatureContent);
}

// ============================================================================
// Inicialización de navegación
// ============================================================================

void initNavigation() {
    extern lv_obj_t *scr_dashboard;

    // Habilitar swipe en dashboard
    enableSwipeNavigation(scr_dashboard);

    // Crear indicador de página
    createPageIndicator(scr_dashboard);

    Serial.println("[NAV] Sistema de navegación inicializado");
}

#endif // UI_NAVIGATION_H
