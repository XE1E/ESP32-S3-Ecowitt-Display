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
#include "ui_detail_principal.h"

// Forward declarations
void createHistoryScreen();
void createSettingsScreen();
void createDetailLocal();
void createDetailJardin();
void updateDetailJardin();
void createDetailRemoto();
void updateDetailRemoto();

// ============================================================================
// Pantallas disponibles
// ============================================================================

typedef enum {
    SCREEN_DASHBOARD = 0,
    SCREEN_DETAIL_PRINCIPAL,
    SCREEN_DETAIL_LOCAL,
    SCREEN_DETAIL_JARDIN,
    SCREEN_DETAIL_REMOTO,
    SCREEN_HISTORY,
    SCREEN_SETTINGS,
    SCREEN_COUNT
} ScreenType;

static ScreenType currentScreen = SCREEN_DASHBOARD;

// Referencias a pantallas (definidas en sus respectivos headers)
extern lv_obj_t *scr_dashboard;
static lv_obj_t *scr_detail_principal = nullptr;
static lv_obj_t *scr_detail_local = nullptr;
static lv_obj_t *scr_detail_jardin = nullptr;
static lv_obj_t *scr_detail_remoto = nullptr;
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
        case SCREEN_DETAIL_PRINCIPAL:
            if (!getDetailPrincipalScreen()) createDetailPrincipal();
            updateDetailPrincipal();  // Actualizar datos antes de mostrar
            target = getDetailPrincipalScreen();
            break;
        case SCREEN_DETAIL_LOCAL:
            if (!scr_detail_local) createDetailLocal();
            target = scr_detail_local;
            break;
        case SCREEN_DETAIL_JARDIN:
            if (!scr_detail_jardin) createDetailJardin();
            updateDetailJardin();
            target = scr_detail_jardin;
            break;
        case SCREEN_DETAIL_REMOTO:
            if (!scr_detail_remoto) createDetailRemoto();
            updateDetailRemoto();  // Actualizar datos antes de mostrar
            target = scr_detail_remoto;
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

// Version con int para evitar dependencias circulares
void navigateToScreenById(int screenId) {
    navigateToScreen((ScreenType)screenId);
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
    lv_obj_align(page_indicator, LV_ALIGN_BOTTOM_MID, 0, -38);
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
    lv_indev_t *indev = lv_indev_get_act();
    if (!indev) return;

    // Ignorar gestos que empezaron en sliders o switches
    lv_obj_t *target = lv_indev_get_obj_act();
    if (target) {
        // Verificar si es slider, switch o su contenedor
        if (lv_obj_check_type(target, &lv_slider_class) ||
            lv_obj_check_type(target, &lv_switch_class)) {
            return;  // No procesar swipe en controles interactivos
        }
        // Verificar ancestros (por si tocamos el knob del slider)
        lv_obj_t *parent = lv_obj_get_parent(target);
        while (parent) {
            if (lv_obj_check_type(parent, &lv_slider_class) ||
                lv_obj_check_type(parent, &lv_switch_class)) {
                return;
            }
            parent = lv_obj_get_parent(parent);
        }
    }

    lv_dir_t dir = lv_indev_get_gesture_dir(indev);

    if (dir == LV_DIR_LEFT) {
        navigateNext();
        updatePageIndicator();
    } else if (dir == LV_DIR_RIGHT) {
        navigatePrev();
        updatePageIndicator();
    }
}

void enableSwipeNavigation(lv_obj_t *screen) {
    lv_obj_add_flag(screen, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scroll_dir(screen, LV_DIR_NONE);  // Disable scroll, enable gesture
    lv_obj_add_event_cb(screen, gestureEventHandler, LV_EVENT_GESTURE, NULL);
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
static lv_obj_t *lbl_history_stats_temp = nullptr;
static lv_obj_t *lbl_history_stats_hum = nullptr;
static lv_obj_t *lbl_history_stats_pres = nullptr;
static lv_obj_t *lbl_history_status = nullptr;

// Datos históricos globales
static HistoryData g_history = {0};

// Forward declaration
void updateHistoryCharts();

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

    // Botón actualizar
    lv_obj_t *btn_refresh = lv_btn_create(scr_history);
    lv_obj_set_size(btn_refresh, 100, 40);
    lv_obj_align(btn_refresh, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_set_style_bg_color(btn_refresh, lv_color_hex(0x2196F3), 0);
    lv_obj_add_event_cb(btn_refresh, [](lv_event_t *e) {
        if (lbl_history_status) {
            lv_label_set_text(lbl_history_status, "Cargando...");
        }
        lv_timer_create([](lv_timer_t *t) {
            updateHistoryCharts();
            lv_timer_del(t);
        }, 100, NULL);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_ref_label = lv_label_create(btn_refresh);
    lv_label_set_text(btn_ref_label, LV_SYMBOL_REFRESH " Actualizar");
    lv_obj_set_style_text_color(btn_ref_label, lv_color_white(), 0);
    lv_obj_center(btn_ref_label);

    // Gráfica de temperatura
    lv_obj_t *card_temp = lv_obj_create(scr_history);
    lv_obj_set_size(card_temp, 480, 200);
    lv_obj_set_pos(card_temp, 15, 60);
    lv_obj_add_style(card_temp, &style_card, 0);
    lv_obj_clear_flag(card_temp, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_temp = lv_label_create(card_temp);
    lv_label_set_text(lbl_temp, "TEMPERATURA (°C)");
    lv_obj_set_style_text_font(lbl_temp, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_temp, lv_color_hex(0x666666), 0);
    lv_obj_align(lbl_temp, LV_ALIGN_TOP_LEFT, 5, 5);

    lbl_history_stats_temp = lv_label_create(card_temp);
    lv_label_set_text(lbl_history_stats_temp, "Min: --  Max: --  Avg: --");
    lv_obj_set_style_text_font(lbl_history_stats_temp, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(lbl_history_stats_temp, lv_color_hex(0xFF6B6B), 0);
    lv_obj_align(lbl_history_stats_temp, LV_ALIGN_TOP_RIGHT, -5, 5);

    chart_temp = lv_chart_create(card_temp);
    lv_obj_set_size(chart_temp, 450, 145);
    lv_obj_align(chart_temp, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_chart_set_type(chart_temp, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_temp, HISTORY_MAX_POINTS);
    lv_chart_set_range(chart_temp, LV_CHART_AXIS_PRIMARY_Y, 10, 40);
    lv_obj_set_style_line_width(chart_temp, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(chart_temp, 0, LV_PART_INDICATOR);  // Sin puntos
    lv_chart_set_div_line_count(chart_temp, 5, 8);
    ser_temp = lv_chart_add_series(chart_temp, lv_color_hex(0xFF6B6B), LV_CHART_AXIS_PRIMARY_Y);

    // Gráfica de humedad
    lv_obj_t *card_hum = lv_obj_create(scr_history);
    lv_obj_set_size(card_hum, 480, 200);
    lv_obj_set_pos(card_hum, 510, 60);
    lv_obj_add_style(card_hum, &style_card, 0);
    lv_obj_clear_flag(card_hum, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_hum = lv_label_create(card_hum);
    lv_label_set_text(lbl_hum, "HUMEDAD (%)");
    lv_obj_set_style_text_font(lbl_hum, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_hum, lv_color_hex(0x666666), 0);
    lv_obj_align(lbl_hum, LV_ALIGN_TOP_LEFT, 5, 5);

    lbl_history_stats_hum = lv_label_create(card_hum);
    lv_label_set_text(lbl_history_stats_hum, "Min: --  Max: --");
    lv_obj_set_style_text_font(lbl_history_stats_hum, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(lbl_history_stats_hum, lv_color_hex(0x2196F3), 0);
    lv_obj_align(lbl_history_stats_hum, LV_ALIGN_TOP_RIGHT, -5, 5);

    chart_humidity = lv_chart_create(card_hum);
    lv_obj_set_size(chart_humidity, 450, 145);
    lv_obj_align(chart_humidity, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_chart_set_type(chart_humidity, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_humidity, HISTORY_MAX_POINTS);
    lv_chart_set_range(chart_humidity, LV_CHART_AXIS_PRIMARY_Y, 0, 100);
    lv_obj_set_style_line_width(chart_humidity, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(chart_humidity, 0, LV_PART_INDICATOR);
    lv_chart_set_div_line_count(chart_humidity, 5, 8);
    ser_humidity = lv_chart_add_series(chart_humidity, lv_color_hex(0x2196F3), LV_CHART_AXIS_PRIMARY_Y);

    // Gráfica de presión
    lv_obj_t *card_pres = lv_obj_create(scr_history);
    lv_obj_set_size(card_pres, 480, 200);
    lv_obj_set_pos(card_pres, 15, 275);
    lv_obj_add_style(card_pres, &style_card, 0);
    lv_obj_clear_flag(card_pres, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_pres = lv_label_create(card_pres);
    lv_label_set_text(lbl_pres, "PRESION (hPa)");
    lv_obj_set_style_text_font(lbl_pres, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_pres, lv_color_hex(0x666666), 0);
    lv_obj_align(lbl_pres, LV_ALIGN_TOP_LEFT, 5, 5);

    lbl_history_stats_pres = lv_label_create(card_pres);
    lv_label_set_text(lbl_history_stats_pres, "Min: --  Max: --");
    lv_obj_set_style_text_font(lbl_history_stats_pres, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(lbl_history_stats_pres, lv_color_hex(0x4CAF50), 0);
    lv_obj_align(lbl_history_stats_pres, LV_ALIGN_TOP_RIGHT, -5, 5);

    chart_pressure = lv_chart_create(card_pres);
    lv_obj_set_size(chart_pressure, 450, 145);
    lv_obj_align(chart_pressure, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_chart_set_type(chart_pressure, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_pressure, HISTORY_MAX_POINTS);
    lv_chart_set_range(chart_pressure, LV_CHART_AXIS_PRIMARY_Y, 990, 1040);
    lv_obj_set_style_line_width(chart_pressure, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(chart_pressure, 0, LV_PART_INDICATOR);
    lv_chart_set_div_line_count(chart_pressure, 5, 8);
    ser_pressure = lv_chart_add_series(chart_pressure, lv_color_hex(0x4CAF50), LV_CHART_AXIS_PRIMARY_Y);

    // Panel de estado/info
    lv_obj_t *card_stats = lv_obj_create(scr_history);
    lv_obj_set_size(card_stats, 480, 200);
    lv_obj_set_pos(card_stats, 510, 275);
    lv_obj_add_style(card_stats, &style_card, 0);
    lv_obj_clear_flag(card_stats, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *lbl_stats = lv_label_create(card_stats);
    lv_label_set_text(lbl_stats, "RESUMEN 24 HORAS");
    lv_obj_set_style_text_font(lbl_stats, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_stats, lv_color_hex(0x666666), 0);
    lv_obj_align(lbl_stats, LV_ALIGN_TOP_LEFT, 5, 5);

    lbl_history_status = lv_label_create(card_stats);
    lv_label_set_text(lbl_history_status, "Presiona Actualizar para cargar datos");
    lv_obj_set_style_text_font(lbl_history_status, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_history_status, lv_color_hex(0x2C2C2C), 0);
    lv_obj_align(lbl_history_status, LV_ALIGN_CENTER, 0, 0);

    // Habilitar swipe
    enableSwipeNavigation(scr_history);

    Serial.println("[UI] Pantalla historial creada");
}

// Actualizar gráficas con datos reales del servidor
void updateHistoryCharts() {
    extern EcowittAPI ecowittApi;

    Serial.println("[HISTORY] Obteniendo datos...");

    if (ecowittApi.fetchHistory(g_history, 24)) {
        Serial.printf("[HISTORY] Recibidos %d puntos\n", g_history.count);

        // Limpiar series
        lv_chart_set_all_value(chart_temp, ser_temp, LV_CHART_POINT_NONE);
        lv_chart_set_all_value(chart_humidity, ser_humidity, LV_CHART_POINT_NONE);
        lv_chart_set_all_value(chart_pressure, ser_pressure, LV_CHART_POINT_NONE);

        // Ajustar rango de temperatura dinámicamente
        int temp_min_range = (int)(g_history.temp_min - 2);
        int temp_max_range = (int)(g_history.temp_max + 2);
        lv_chart_set_range(chart_temp, LV_CHART_AXIS_PRIMARY_Y, temp_min_range, temp_max_range);

        // Ajustar rango de presión dinámicamente
        int pres_min_range = (int)(g_history.pres_min - 5);
        int pres_max_range = (int)(g_history.pres_max + 5);
        lv_chart_set_range(chart_pressure, LV_CHART_AXIS_PRIMARY_Y, pres_min_range, pres_max_range);

        // Rellenar datos (del más antiguo al más reciente)
        for (int i = g_history.count - 1; i >= 0; i--) {
            lv_chart_set_next_value(chart_temp, ser_temp, (lv_coord_t)g_history.points[i].temperature);
            lv_chart_set_next_value(chart_humidity, ser_humidity, (lv_coord_t)g_history.points[i].humidity);
            lv_chart_set_next_value(chart_pressure, ser_pressure, (lv_coord_t)g_history.points[i].pressure);
        }

        lv_chart_refresh(chart_temp);
        lv_chart_refresh(chart_humidity);
        lv_chart_refresh(chart_pressure);

        // Actualizar estadísticas
        char buf[64];
        snprintf(buf, sizeof(buf), "Min: %.1f  Max: %.1f  Avg: %.1f",
                 g_history.temp_min, g_history.temp_max, g_history.temp_avg);
        lv_label_set_text(lbl_history_stats_temp, buf);

        snprintf(buf, sizeof(buf), "Min: %.0f%%  Max: %.0f%%",
                 g_history.hum_min, g_history.hum_max);
        lv_label_set_text(lbl_history_stats_hum, buf);

        snprintf(buf, sizeof(buf), "Min: %.0f  Max: %.0f",
                 g_history.pres_min, g_history.pres_max);
        lv_label_set_text(lbl_history_stats_pres, buf);

        snprintf(buf, sizeof(buf), "%d puntos de datos\nTemp: %.1f a %.1f°C\nHum: %.0f a %.0f%%\nPres: %.0f a %.0f hPa",
                 g_history.count,
                 g_history.temp_min, g_history.temp_max,
                 g_history.hum_min, g_history.hum_max,
                 g_history.pres_min, g_history.pres_max);
        lv_label_set_text(lbl_history_status, buf);

    } else {
        lv_label_set_text(lbl_history_status, "Error al cargar datos\nVerifica conexion");
        Serial.println("[HISTORY] Error al obtener datos");
    }
}

// ============================================================================
// Pantalla de Configuración
// ============================================================================

static lv_obj_t *sw_24h = nullptr;
static lv_obj_t *slider_brightness = nullptr;
static lv_obj_t *slider_pressure_offset = nullptr;
static lv_obj_t *lbl_pressure_offset_val = nullptr;
static lv_obj_t *slider_temperature_offset = nullptr;
static lv_obj_t *lbl_temperature_offset_val = nullptr;

void createSettingsScreen() {
    extern bool darkMode;

    // Paleta de 3 tonos grises (consistente con otras pantallas)
    lv_color_t bg_dark = lv_color_hex(0x0A0E12);      // Tono 1: fondo
    lv_color_t card_dark = lv_color_hex(0x1A1F24);    // Tono 2: cards
    lv_color_t subcard_dark = lv_color_hex(0x2A3238); // Tono 3: sub-cards/botones
    lv_color_t border_dark = lv_color_hex(0x3A4248);
    lv_color_t text_primary = lv_color_hex(0xF1F5F9);
    lv_color_t text_muted = lv_color_hex(0x94A3B8);
    lv_color_t accent_blue = lv_color_hex(0x60A5FA);
    lv_color_t accent_green = lv_color_hex(0x4ADE80);
    lv_color_t accent_red = lv_color_hex(0xF87171);

    scr_settings = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_settings, bg_dark, 0);
    lv_obj_set_style_bg_opa(scr_settings, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr_settings, 0, 0);
    lv_obj_clear_flag(scr_settings, LV_OBJ_FLAG_SCROLLABLE);

    // === HEADER ===
    lv_obj_t *header = lv_obj_create(scr_settings);
    lv_obj_set_size(header, SCREEN_WIDTH, 56);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, card_dark, 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(header, border_dark, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    // Botón volver
    lv_obj_t *btn_back = lv_btn_create(scr_settings);
    lv_obj_set_size(btn_back, 90, 32);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 12, 12);
    lv_obj_set_style_bg_color(btn_back, subcard_dark, 0);
    lv_obj_set_style_radius(btn_back, 6, 0);
    lv_obj_set_style_shadow_width(btn_back, 0, 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        navigateToScreen(SCREEN_DASHBOARD);
        updatePageIndicator();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn_back);
    lv_label_set_text(btn_label, LV_SYMBOL_LEFT " Volver");
    lv_obj_set_style_text_color(btn_label, text_primary, 0);
    lv_obj_center(btn_label);

    // Título
    lv_obj_t *title = lv_label_create(scr_settings);
    lv_label_set_text(title, LV_SYMBOL_SETTINGS " CONFIGURACION");
    lv_obj_set_style_text_color(title, text_primary, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 18);

    // Layout
    int gap = 10;
    int y_start = 66;
    int card_w = 495;
    int card_h = 260;

    // Helper para crear cards estilo glassmorphism
    auto createCard = [&](int x, int y, int w, int h, const char* title_text) -> lv_obj_t* {
        lv_obj_t *card = lv_obj_create(scr_settings);
        lv_obj_set_pos(card, x, y);
        lv_obj_set_size(card, w, h);
        lv_obj_set_style_bg_color(card, card_dark, 0);
        lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
        lv_obj_set_style_radius(card, 16, 0);
        lv_obj_set_style_border_width(card, 1, 0);
        lv_obj_set_style_border_color(card, border_dark, 0);
        lv_obj_set_style_pad_all(card, 12, 0);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *lbl = lv_label_create(card);
        lv_label_set_text(lbl, title_text);
        lv_obj_set_style_text_color(lbl, text_muted, 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
        lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 0, 0);

        return card;
    };

    // === CARD 1: AJUSTES (izquierda) ===
    lv_obj_t *card_units = createCard(gap, y_start, card_w, card_h, "AJUSTES");

    // Formato hora
    lv_obj_t *lbl_time = lv_label_create(card_units);
    lv_label_set_text(lbl_time, "Formato hora:");
    lv_obj_set_style_text_color(lbl_time, text_primary, 0);
    lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_time, LV_ALIGN_TOP_LEFT, 0, 28);

    sw_24h = lv_switch_create(card_units);
    lv_obj_align(sw_24h, LV_ALIGN_TOP_LEFT, 160, 25);
    if (userPrefs.use_24h) lv_obj_add_state(sw_24h, LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw_24h, subcard_dark, 0);
    lv_obj_set_style_bg_color(sw_24h, accent_blue, LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_t *lbl_12h = lv_label_create(card_units);
    lv_label_set_text(lbl_12h, "12h");
    lv_obj_set_style_text_color(lbl_12h, text_muted, 0);
    lv_obj_align_to(lbl_12h, sw_24h, LV_ALIGN_OUT_LEFT_MID, -10, 0);

    lv_obj_t *lbl_24h = lv_label_create(card_units);
    lv_label_set_text(lbl_24h, "24h");
    lv_obj_set_style_text_color(lbl_24h, text_muted, 0);
    lv_obj_align_to(lbl_24h, sw_24h, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_add_event_cb(sw_24h, [](lv_event_t *e) {
        lv_obj_t *sw = lv_event_get_target(e);
        userPrefs.use_24h = lv_obj_has_state(sw, LV_STATE_CHECKED);
        savePreferences();
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Brillo pantalla
    lv_obj_t *lbl_bright = lv_label_create(card_units);
    lv_label_set_text(lbl_bright, "Brillo pantalla:");
    lv_obj_set_style_text_color(lbl_bright, text_primary, 0);
    lv_obj_set_style_text_font(lbl_bright, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_bright, LV_ALIGN_TOP_LEFT, 0, 68);

    slider_brightness = lv_slider_create(card_units);
    lv_obj_set_width(slider_brightness, 200);
    lv_obj_align(slider_brightness, LV_ALIGN_TOP_LEFT, 160, 73);
    lv_slider_set_range(slider_brightness, 10, 100);
    lv_slider_set_value(slider_brightness, userPrefs.brightness * 100 / 255, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider_brightness, subcard_dark, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_brightness, accent_blue, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_brightness, text_primary, LV_PART_KNOB);

    lv_obj_t *lbl_bright_val = lv_label_create(card_units);
    char bright_buf[10];
    snprintf(bright_buf, sizeof(bright_buf), "%d%%", userPrefs.brightness * 100 / 255);
    lv_label_set_text(lbl_bright_val, bright_buf);
    lv_obj_set_style_text_color(lbl_bright_val, accent_blue, 0);
    lv_obj_align_to(lbl_bright_val, slider_brightness, LV_ALIGN_OUT_RIGHT_MID, 15, 0);

    lv_obj_add_event_cb(slider_brightness, [](lv_event_t *e) {
        lv_obj_t *slider = lv_event_get_target(e);
        int val = lv_slider_get_value(slider);
        setBacklight(val);
        lv_obj_t *parent = lv_obj_get_parent(slider);
        lv_obj_t *label = lv_obj_get_child(parent, 7);  // brightness value label
        if (label) {
            char buf[10];
            snprintf(buf, sizeof(buf), "%d%%", val);
            lv_label_set_text(label, buf);
        }
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // --- BME280 Calibracion ---
    lv_obj_t *lbl_bme_section = lv_label_create(card_units);
    lv_label_set_text(lbl_bme_section, "CALIBRACION BME280");
    lv_obj_set_style_text_color(lbl_bme_section, text_muted, 0);
    lv_obj_set_style_text_font(lbl_bme_section, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_bme_section, LV_ALIGN_TOP_LEFT, 0, 115);

    // Offset temperatura
    lv_obj_t *lbl_temp_off = lv_label_create(card_units);
    lv_label_set_text(lbl_temp_off, "Offset temp:");
    lv_obj_set_style_text_color(lbl_temp_off, text_primary, 0);
    lv_obj_set_style_text_font(lbl_temp_off, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_temp_off, LV_ALIGN_TOP_LEFT, 0, 140);

    slider_temperature_offset = lv_slider_create(card_units);
    lv_obj_set_width(slider_temperature_offset, 200);
    lv_obj_align(slider_temperature_offset, LV_ALIGN_TOP_LEFT, 140, 145);
    lv_slider_set_range(slider_temperature_offset, -15, 15);
    lv_slider_set_value(slider_temperature_offset, userPrefs.bme280_temperature_offset, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider_temperature_offset, subcard_dark, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_temperature_offset, accent_blue, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_temperature_offset, text_primary, LV_PART_KNOB);

    lbl_temperature_offset_val = lv_label_create(card_units);
    char temp_off_buf[16];
    snprintf(temp_off_buf, sizeof(temp_off_buf), "%+d C", userPrefs.bme280_temperature_offset);
    lv_label_set_text(lbl_temperature_offset_val, temp_off_buf);
    lv_obj_set_style_text_color(lbl_temperature_offset_val, accent_blue, 0);
    lv_obj_align_to(lbl_temperature_offset_val, slider_temperature_offset, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_add_event_cb(slider_temperature_offset, [](lv_event_t *e) {
        lv_obj_t *slider = lv_event_get_target(e);
        int val = lv_slider_get_value(slider);
        userPrefs.bme280_temperature_offset = (int8_t)val;
        setBME280TemperatureOffset((int8_t)val);
        savePreferences();
        if (lbl_temperature_offset_val) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%+d C", val);
            lv_label_set_text(lbl_temperature_offset_val, buf);
        }
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Offset presión
    lv_obj_t *lbl_pres_off = lv_label_create(card_units);
    lv_label_set_text(lbl_pres_off, "Offset presion:");
    lv_obj_set_style_text_color(lbl_pres_off, text_primary, 0);
    lv_obj_set_style_text_font(lbl_pres_off, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_pres_off, LV_ALIGN_TOP_LEFT, 0, 185);

    slider_pressure_offset = lv_slider_create(card_units);
    lv_obj_set_width(slider_pressure_offset, 200);
    lv_obj_align(slider_pressure_offset, LV_ALIGN_TOP_LEFT, 140, 190);
    lv_slider_set_range(slider_pressure_offset, -15, 15);
    lv_slider_set_value(slider_pressure_offset, userPrefs.bme280_pressure_offset, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider_pressure_offset, subcard_dark, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider_pressure_offset, accent_blue, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_pressure_offset, text_primary, LV_PART_KNOB);

    lbl_pressure_offset_val = lv_label_create(card_units);
    char off_buf[16];
    snprintf(off_buf, sizeof(off_buf), "%+d hPa", userPrefs.bme280_pressure_offset);
    lv_label_set_text(lbl_pressure_offset_val, off_buf);
    lv_obj_set_style_text_color(lbl_pressure_offset_val, accent_blue, 0);
    lv_obj_align_to(lbl_pressure_offset_val, slider_pressure_offset, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    lv_obj_add_event_cb(slider_pressure_offset, [](lv_event_t *e) {
        lv_obj_t *slider = lv_event_get_target(e);
        int val = lv_slider_get_value(slider);
        userPrefs.bme280_pressure_offset = (int8_t)val;
        setBME280PressureOffset((int8_t)val);
        savePreferences();
        if (lbl_pressure_offset_val) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%+d hPa", val);
            lv_label_set_text(lbl_pressure_offset_val, buf);
        }
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // === CARD 2: SISTEMA (derecha) ===
    lv_obj_t *card_conn = createCard(gap + card_w + gap, y_start, card_w, card_h, "SISTEMA");

    extern SystemStatus g_status;

    // Estado de conexiones en líneas separadas
    lv_obj_t *lbl_wifi = lv_label_create(card_conn);
    char wifi_txt[64];
    snprintf(wifi_txt, sizeof(wifi_txt), "WiFi: %s (%d dBm)",
             g_status.wifi_connected ? "Conectado" : "Desconectado",
             g_status.wifi_rssi);
    lv_label_set_text(lbl_wifi, wifi_txt);
    lv_obj_set_style_text_color(lbl_wifi, g_status.wifi_connected ? accent_green : accent_red, 0);
    lv_obj_set_style_text_font(lbl_wifi, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_wifi, LV_ALIGN_TOP_LEFT, 0, 18);

    lv_obj_t *lbl_api = lv_label_create(card_conn);
    lv_label_set_text(lbl_api, g_status.api_ok ? "Servidor: OK" : "Servidor: ERROR");
    lv_obj_set_style_text_color(lbl_api, g_status.api_ok ? accent_green : accent_red, 0);
    lv_obj_set_style_text_font(lbl_api, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_api, LV_ALIGN_TOP_LEFT, 200, 18);

    lv_obj_t *lbl_bme = lv_label_create(card_conn);
    lv_label_set_text(lbl_bme, g_status.bme280_ok ? "BME280: OK" : "BME280: --");
    lv_obj_set_style_text_color(lbl_bme, g_status.bme280_ok ? accent_green : text_muted, 0);
    lv_obj_set_style_text_font(lbl_bme, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_bme, LV_ALIGN_TOP_LEFT, 340, 18);

    // Helper para crear barra de recursos compacta
    auto createResourceBar = [&](lv_obj_t* parent, int y, const char* label,
                                  uint32_t used, uint32_t total, const char* unit) {
        int bar_w = 250;
        int bar_h = 12;
        int pct = (total > 0) ? (used * 100 / total) : 0;

        lv_obj_t *lbl = lv_label_create(parent);
        lv_label_set_text(lbl, label);
        lv_obj_set_style_text_color(lbl, text_primary, 0);
        lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
        lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 0, y);

        // Barra de fondo
        lv_obj_t *bar_bg = lv_obj_create(parent);
        lv_obj_set_size(bar_bg, bar_w, bar_h);
        lv_obj_align(bar_bg, LV_ALIGN_TOP_LEFT, 55, y + 1);
        lv_obj_set_style_bg_color(bar_bg, subcard_dark, 0);
        lv_obj_set_style_radius(bar_bg, 3, 0);
        lv_obj_set_style_border_width(bar_bg, 0, 0);
        lv_obj_set_style_pad_all(bar_bg, 0, 0);
        lv_obj_clear_flag(bar_bg, LV_OBJ_FLAG_SCROLLABLE);

        // Barra de uso
        lv_obj_t *bar_fill = lv_obj_create(bar_bg);
        int fill_w = (bar_w * pct) / 100;
        if (fill_w < 3) fill_w = 3;
        lv_obj_set_size(bar_fill, fill_w, bar_h);
        lv_obj_set_pos(bar_fill, 0, 0);
        lv_color_t bar_color = (pct > 80) ? accent_red : ((pct > 60) ? lv_color_hex(0xFBBF24) : accent_green);
        lv_obj_set_style_bg_color(bar_fill, bar_color, 0);
        lv_obj_set_style_radius(bar_fill, 3, 0);
        lv_obj_set_style_border_width(bar_fill, 0, 0);
        lv_obj_clear_flag(bar_fill, LV_OBJ_FLAG_SCROLLABLE);

        // Texto de valores
        lv_obj_t *lbl_val = lv_label_create(parent);
        char val_buf[32];
        if (total >= 1024 * 1024) {
            snprintf(val_buf, sizeof(val_buf), "%.1f/%.1f %s", used / 1048576.0f, total / 1048576.0f, unit);
        } else {
            snprintf(val_buf, sizeof(val_buf), "%.0f/%.0f %s", used / 1024.0f, total / 1024.0f, unit);
        }
        lv_label_set_text(lbl_val, val_buf);
        lv_obj_set_style_text_color(lbl_val, text_muted, 0);
        lv_obj_set_style_text_font(lbl_val, &lv_font_montserrat_12, 0);
        lv_obj_align(lbl_val, LV_ALIGN_TOP_LEFT, 315, y);
    };

    // Recursos del sistema
    uint32_t heap_total = ESP.getHeapSize();
    uint32_t heap_used = heap_total - ESP.getFreeHeap();
    uint32_t psram_total = ESP.getPsramSize();
    uint32_t psram_used = psram_total - ESP.getFreePsram();
    uint32_t flash_total = ESP.getFlashChipSize();
    uint32_t flash_used = ESP.getSketchSize();

    createResourceBar(card_conn, 40, "RAM", heap_used, heap_total, "KB");
    createResourceBar(card_conn, 62, "PSRAM", psram_used, psram_total, "MB");
    createResourceBar(card_conn, 84, "Flash", flash_used, flash_total, "MB");

    // Uptime y versión
    lv_obj_t *lbl_uptime = lv_label_create(card_conn);
    unsigned long uptime_min = millis() / 60000;
    char uptime_txt[32];
    if (uptime_min >= 60) {
        snprintf(uptime_txt, sizeof(uptime_txt), "Uptime: %luh %lum", uptime_min / 60, uptime_min % 60);
    } else {
        snprintf(uptime_txt, sizeof(uptime_txt), "Uptime: %lu min", uptime_min);
    }
    lv_label_set_text(lbl_uptime, uptime_txt);
    lv_obj_set_style_text_color(lbl_uptime, text_muted, 0);
    lv_obj_set_style_text_font(lbl_uptime, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_uptime, LV_ALIGN_TOP_LEFT, 0, 110);

    lv_obj_t *lbl_version = lv_label_create(card_conn);
    lv_label_set_text(lbl_version, "Version: 1.0.0");
    lv_obj_set_style_text_color(lbl_version, text_muted, 0);
    lv_obj_set_style_text_font(lbl_version, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_version, LV_ALIGN_TOP_LEFT, 150, 110);

    // Botón reiniciar
    lv_obj_t *btn_restart = lv_btn_create(card_conn);
    lv_obj_set_size(btn_restart, 140, 36);
    lv_obj_align(btn_restart, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(btn_restart, accent_red, 0);
    lv_obj_set_style_radius(btn_restart, 8, 0);
    lv_obj_add_event_cb(btn_restart, [](lv_event_t *e) {
        ESP.restart();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_restart = lv_label_create(btn_restart);
    lv_label_set_text(lbl_restart, LV_SYMBOL_REFRESH " Reiniciar");
    lv_obj_set_style_text_color(lbl_restart, lv_color_white(), 0);
    lv_obj_center(lbl_restart);

    // Footer con swipe indicator
    lv_obj_t *footer = lv_label_create(scr_settings);
    lv_label_set_text(footer, LV_SYMBOL_LEFT " Swipe " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(footer, text_muted, 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -10);

    // Habilitar swipe
    enableSwipeNavigation(scr_settings);

    Serial.println("[UI] Pantalla configuración creada (estilo dark)");
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

    // Sensacion termica
    lv_obj_t *lbl_feels = lv_label_create(parent);
    snprintf(buf, sizeof(buf), "Sensacion termica: %.1f°C", g_weather.feels_like);
    lv_label_set_text(lbl_feels, buf);
    lv_obj_set_style_text_font(lbl_feels, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_feels, LV_ALIGN_TOP_LEFT, 20, 120);

    // Punto de rocio
    lv_obj_t *lbl_dew = lv_label_create(parent);
    snprintf(buf, sizeof(buf), "Punto de rocio: %.1f°C", g_weather.dewpoint);
    lv_label_set_text(lbl_dew, buf);
    lv_obj_set_style_text_font(lbl_dew, &lv_font_montserrat_18, 0);
    lv_obj_align(lbl_dew, LV_ALIGN_TOP_LEFT, 20, 150);

    // Maxima/Minima
    lv_obj_t *lbl_range = lv_label_create(parent);
    snprintf(buf, sizeof(buf), "Hoy: Max %.1f°C  /  Min %.1f°C",
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
    lv_label_set_text(lbl_chart, "Ultimas 24 horas");
    lv_obj_set_style_text_font(lbl_chart, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(lbl_chart, lv_color_hex(0x999999), 0);
    lv_obj_align(lbl_chart, LV_ALIGN_BOTTOM_RIGHT, -120, -10);

    return parent;
}

void showTemperatureModal() {
    showModal("TEMPERATURA", modalTemperatureContent);
}

// ============================================================================
// Pantallas de Detalle (tap en cards del dashboard)
// ============================================================================

#include "weather_icons.h"

// Colores para cada ubicacion - alineados con tema
#define DETAIL_COLOR_PRINCIPAL  lv_color_hex(0x60A5FA)  // blue-400
#define DETAIL_COLOR_LOCAL      lv_color_hex(0x4ADE80)  // green-400
#define DETAIL_COLOR_JARDIN     lv_color_hex(0x7BC8A4)  // weather-wind
#define DETAIL_COLOR_REMOTO     lv_color_hex(0xA78BFA)  // violet-400

// Colores de texto para detalles (accesibles desde ui_dashboard.h)
extern bool darkMode;
#define DETAIL_TEXT_PRIMARY     (darkMode ? lv_color_hex(0xE8EDF5) : lv_color_hex(0x16232F))
#define DETAIL_TEXT_SECONDARY   (darkMode ? lv_color_hex(0xCBD5E1) : lv_color_hex(0x475569))
#define DETAIL_TEXT_MUTED       (darkMode ? lv_color_hex(0x8496A6) : lv_color_hex(0x55677A))
// Paleta de 3 tonos grises uniforme (sin tintes verdes/azules)
// Tono 1: casi negro (fondo) - 0x0A0E12
// Tono 2: gris oscuro (cards, header) - 0x1A1F24
// Tono 3: gris medio (sub-cards, botones) - 0x2A3238
#define DETAIL_BG               (darkMode ? lv_color_hex(0x0A0E12) : lv_color_hex(0xF8FAFC))
#define DETAIL_CARD_BG          (darkMode ? lv_color_hex(0x1A1F24) : lv_color_hex(0xFFFFFF))
#define DETAIL_HEADER_BG        (darkMode ? lv_color_hex(0x1A1F24) : lv_color_hex(0xFFFFFF))
#define DETAIL_SUBCARD_BG       (darkMode ? lv_color_hex(0x2A3238) : lv_color_hex(0xF1F5F9))
#define DETAIL_BORDER           (darkMode ? lv_color_hex(0x3A4248) : lv_color_hex(0xE2E8F0))

// Boton volver generico - Tono 3 (gris medio)
lv_obj_t* createBackButton(lv_obj_t *parent) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, 100, 36);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 15, 17);
    lv_obj_set_style_bg_color(btn, DETAIL_SUBCARD_BG, 0);  // Tono 3
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, DETAIL_BORDER, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        navigateToScreen(SCREEN_DASHBOARD);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl = lv_label_create(btn);
    lv_label_set_text(lbl, LV_SYMBOL_LEFT " Volver");
    lv_obj_set_style_text_color(lbl, DETAIL_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
    lv_obj_center(lbl);
    return btn;
}

// Header de pantalla de detalle - estilo servidor
void createDetailHeader(lv_obj_t *parent, const char* title, const char* subtitle, lv_color_t accent) {
    // Fondo del header
    lv_obj_t *header_bg = lv_obj_create(parent);
    lv_obj_set_size(header_bg, SCREEN_WIDTH, 70);
    lv_obj_set_pos(header_bg, 0, 0);
    lv_obj_set_style_bg_color(header_bg, DETAIL_HEADER_BG, 0);
    lv_obj_set_style_radius(header_bg, 0, 0);
    lv_obj_set_style_border_width(header_bg, 1, 0);
    lv_obj_set_style_border_side(header_bg, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(header_bg, DETAIL_BORDER, 0);
    lv_obj_clear_flag(header_bg, LV_OBJ_FLAG_SCROLLABLE);

    createBackButton(parent);

    // Titulo
    lv_obj_t *lbl_title = lv_label_create(parent);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_color(lbl_title, DETAIL_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_24, 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_MID, 0, 15);

    // Subtitulo
    lv_obj_t *lbl_sub = lv_label_create(parent);
    lv_label_set_text(lbl_sub, subtitle);
    lv_obj_set_style_text_color(lbl_sub, accent, 0);
    lv_obj_set_style_text_font(lbl_sub, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_sub, LV_ALIGN_TOP_MID, 0, 45);
}

// Card generico para pantalla de detalle - estilo servidor
lv_obj_t* createDetailCard(lv_obj_t *parent, int x, int y, int w, int h, const char* title, lv_color_t accent) {
    extern lv_style_t style_card;

    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    // Barra de acento superior
    lv_obj_t *accent_bar = lv_obj_create(card);
    lv_obj_set_size(accent_bar, w - 24, 3);
    lv_obj_align(accent_bar, LV_ALIGN_TOP_MID, 0, -8);
    lv_obj_set_style_bg_color(accent_bar, accent, 0);
    lv_obj_set_style_bg_opa(accent_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(accent_bar, 2, 0);
    lv_obj_set_style_border_width(accent_bar, 0, 0);
    lv_obj_clear_flag(accent_bar, LV_OBJ_FLAG_SCROLLABLE);

    // Titulo con color muted
    lv_obj_t *lbl_title = lv_label_create(card);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_color(lbl_title, DETAIL_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_LEFT, 10, 8);

    return card;
}

/**
 * Pantalla detalle LOCAL (BME280)
 */
void createDetailLocal() {
    extern LocalSensorData g_local;

    scr_detail_local = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_detail_local, DETAIL_BG, 0);
    lv_obj_clear_flag(scr_detail_local, LV_OBJ_FLAG_SCROLLABLE);

    createDetailHeader(scr_detail_local, "SENSOR LOCAL", "BME280 Interior", DETAIL_COLOR_LOCAL);

    int y_start = 85;
    int gap = 12;

    // Card grande con todos los datos
    lv_obj_t *card = createDetailCard(scr_detail_local, gap, y_start, SCREEN_WIDTH - gap * 2, 250, "LECTURAS ACTUALES", DETAIL_COLOR_LOCAL);

    // Temperatura
    lv_obj_t *temp_icon = lv_label_create(card);
    lv_label_set_text(temp_icon, WI_THERMOMETER);
    lv_obj_set_style_text_font(temp_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(temp_icon, DETAIL_COLOR_LOCAL, 0);
    lv_obj_align(temp_icon, LV_ALIGN_TOP_LEFT, 20, 40);

    lv_obj_t *temp_val = lv_label_create(card);
    lv_label_set_text(temp_val, "22.5°C");
    lv_obj_set_style_text_color(temp_val, DETAIL_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(temp_val, &lv_font_montserrat_48, 0);
    lv_obj_align(temp_val, LV_ALIGN_TOP_LEFT, 90, 35);

    lv_obj_t *maxmin = lv_label_create(card);
    lv_label_set_text(maxmin, LV_SYMBOL_UP " 25°  " LV_SYMBOL_DOWN " 20°");
    lv_obj_set_style_text_color(maxmin, DETAIL_TEXT_MUTED, 0);
    lv_obj_align(maxmin, LV_ALIGN_TOP_LEFT, 20, 100);

    // Humedad
    lv_obj_t *hum_icon = lv_label_create(card);
    lv_label_set_text(hum_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(hum_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(hum_icon, lv_color_hex(0x67E8F9), 0);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_MID, -100, 40);

    lv_obj_t *hum_val = lv_label_create(card);
    lv_label_set_text(hum_val, "58%");
    lv_obj_set_style_text_color(hum_val, lv_color_hex(0x67E8F9), 0);
    lv_obj_set_style_text_font(hum_val, &lv_font_montserrat_48, 0);
    lv_obj_align(hum_val, LV_ALIGN_TOP_MID, 0, 35);

    // Presion
    lv_obj_t *pres_icon = lv_label_create(card);
    lv_label_set_text(pres_icon, WI_BAROMETER);
    lv_obj_set_style_text_font(pres_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(pres_icon, lv_color_hex(0xC4B5FD), 0);
    lv_obj_align(pres_icon, LV_ALIGN_TOP_RIGHT, -200, 40);

    lv_obj_t *pres_val = lv_label_create(card);
    lv_label_set_text(pres_val, "1018 hPa");
    lv_obj_set_style_text_color(pres_val, lv_color_hex(0xC4B5FD), 0);
    lv_obj_set_style_text_font(pres_val, &lv_font_montserrat_28, 0);
    lv_obj_align(pres_val, LV_ALIGN_TOP_RIGHT, -20, 45);

    // Diferencia vs exterior
    lv_obj_t *diff = lv_label_create(card);
    lv_label_set_text(diff, "vs Exterior: +1.5°C mas calido");
    lv_obj_set_style_text_color(diff, lv_color_hex(0xFDE047), 0);
    lv_obj_set_style_text_font(diff, &lv_font_montserrat_18, 0);
    lv_obj_align(diff, LV_ALIGN_BOTTOM_LEFT, 20, -20);

    // Card de grafica placeholder
    int y2 = y_start + 250 + gap;
    lv_obj_t *card_chart = createDetailCard(scr_detail_local, gap, y2, SCREEN_WIDTH - gap * 2, 220, "HISTORIAL 24 HORAS", DETAIL_COLOR_LOCAL);

    lv_obj_t *chart = lv_chart_create(card_chart);
    lv_obj_set_size(chart, SCREEN_WIDTH - 80, 150);
    lv_obj_align(chart, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 24);
    lv_obj_set_style_line_width(chart, 2, LV_PART_ITEMS);
    lv_chart_series_t *ser = lv_chart_add_series(chart, DETAIL_COLOR_LOCAL, LV_CHART_AXIS_PRIMARY_Y);
    for (int i = 0; i < 24; i++) {
        lv_chart_set_next_value(chart, ser, 20 + (rand() % 5));
    }

    enableSwipeNavigation(scr_detail_local);
    Serial.println("[UI] Pantalla detalle Local creada");
}

/**
 * Pantalla detalle JARDIN (HN31)
 */
// UI elements for Jardin detail
static struct {
    lv_obj_t *temp;
    lv_obj_t *humidity;
    lv_obj_t *maxmin;
    lv_obj_t *battery;
    lv_obj_t *diff;
    lv_obj_t *chart;
    lv_chart_series_t *ser_temp;
    lv_obj_t *lbl_y_max;
    lv_obj_t *lbl_y_min;
} detail_jardin_ui = {0};

void createDetailJardin() {
    extern RemoteSensorData g_jardin;
    extern WeatherData g_weather;

    // Paleta dark
    lv_color_t bg_dark = lv_color_hex(0x0A0E12);
    lv_color_t card_dark = lv_color_hex(0x1A1F24);
    lv_color_t subcard_dark = lv_color_hex(0x2A3238);
    lv_color_t border_dark = lv_color_hex(0x3A4248);
    lv_color_t text_primary = lv_color_hex(0xF1F5F9);
    lv_color_t text_muted = lv_color_hex(0x94A3B8);
    lv_color_t accent_green = lv_color_hex(0x4ADE80);
    lv_color_t accent_cyan = lv_color_hex(0x67E8F9);
    lv_color_t grid_color = lv_color_hex(0x5A6570);

    scr_detail_jardin = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_detail_jardin, bg_dark, 0);
    lv_obj_set_style_bg_opa(scr_detail_jardin, LV_OPA_COVER, 0);
    lv_obj_clear_flag(scr_detail_jardin, LV_OBJ_FLAG_SCROLLABLE);

    // === HEADER ===
    lv_obj_t *header = lv_obj_create(scr_detail_jardin);
    lv_obj_set_size(header, SCREEN_WIDTH, 56);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, card_dark, 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(header, border_dark, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btn_back = lv_btn_create(scr_detail_jardin);
    lv_obj_set_size(btn_back, 90, 32);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 12, 12);
    lv_obj_set_style_bg_color(btn_back, subcard_dark, 0);
    lv_obj_set_style_radius(btn_back, 6, 0);
    lv_obj_set_style_shadow_width(btn_back, 0, 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        navigateToScreen(SCREEN_DASHBOARD);
        updatePageIndicator();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn_back);
    lv_label_set_text(btn_label, LV_SYMBOL_LEFT " Volver");
    lv_obj_set_style_text_color(btn_label, text_primary, 0);
    lv_obj_center(btn_label);

    lv_obj_t *title = lv_label_create(scr_detail_jardin);
    lv_label_set_text(title, "JARDIN");
    lv_obj_set_style_text_color(title, accent_green, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 16);

    lv_obj_t *subtitle = lv_label_create(scr_detail_jardin);
    lv_label_set_text(subtitle, "WN31");
    lv_obj_set_style_text_color(subtitle, text_muted, 0);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_RIGHT, -15, 22);

    int gap = 10;
    int y_start = 66;

    // === CARD DATOS (arriba, a todo lo ancho) ===
    int data_card_h = 180;
    lv_obj_t *card_data = lv_obj_create(scr_detail_jardin);
    lv_obj_set_pos(card_data, gap, y_start);
    lv_obj_set_size(card_data, SCREEN_WIDTH - gap * 2, data_card_h);
    lv_obj_set_style_bg_color(card_data, card_dark, 0);
    lv_obj_set_style_radius(card_data, 16, 0);
    lv_obj_set_style_border_width(card_data, 1, 0);
    lv_obj_set_style_border_color(card_data, border_dark, 0);
    lv_obj_set_style_pad_all(card_data, 12, 0);
    lv_obj_clear_flag(card_data, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *card_title = lv_label_create(card_data);
    lv_label_set_text(card_title, "LECTURAS ACTUALES");
    lv_obj_set_style_text_color(card_title, text_muted, 0);
    lv_obj_set_style_text_font(card_title, &lv_font_montserrat_12, 0);
    lv_obj_align(card_title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Temperatura con icono
    lv_obj_t *temp_icon = lv_label_create(card_data);
    lv_label_set_text(temp_icon, WI_THERMOMETER);
    lv_obj_set_style_text_font(temp_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(temp_icon, accent_green, 0);
    lv_obj_align(temp_icon, LV_ALIGN_TOP_LEFT, 20, 35);

    detail_jardin_ui.temp = lv_label_create(card_data);
    char temp_buf[16];
    snprintf(temp_buf, sizeof(temp_buf), "%.1f°C", g_jardin.valid ? g_jardin.temperature : 0.0f);
    lv_label_set_text(detail_jardin_ui.temp, temp_buf);
    lv_obj_set_style_text_color(detail_jardin_ui.temp, text_primary, 0);
    lv_obj_set_style_text_font(detail_jardin_ui.temp, &lv_font_montserrat_48, 0);
    lv_obj_align(detail_jardin_ui.temp, LV_ALIGN_TOP_LEFT, 90, 30);

    detail_jardin_ui.maxmin = lv_label_create(card_data);
    lv_label_set_text(detail_jardin_ui.maxmin, LV_SYMBOL_UP " --°  " LV_SYMBOL_DOWN " --°");
    lv_obj_set_style_text_color(detail_jardin_ui.maxmin, text_muted, 0);
    lv_obj_set_style_text_font(detail_jardin_ui.maxmin, &lv_font_montserrat_16, 0);
    lv_obj_align(detail_jardin_ui.maxmin, LV_ALIGN_TOP_LEFT, 20, 95);

    // Humedad
    lv_obj_t *hum_icon = lv_label_create(card_data);
    lv_label_set_text(hum_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(hum_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(hum_icon, accent_cyan, 0);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_MID, -80, 35);

    detail_jardin_ui.humidity = lv_label_create(card_data);
    char hum_buf[16];
    snprintf(hum_buf, sizeof(hum_buf), "%.0f%%", g_jardin.valid ? g_jardin.humidity : 0.0f);
    lv_label_set_text(detail_jardin_ui.humidity, hum_buf);
    lv_obj_set_style_text_color(detail_jardin_ui.humidity, accent_cyan, 0);
    lv_obj_set_style_text_font(detail_jardin_ui.humidity, &lv_font_montserrat_48, 0);
    lv_obj_align(detail_jardin_ui.humidity, LV_ALIGN_TOP_MID, 20, 30);

    // Bateria
    detail_jardin_ui.battery = lv_label_create(card_data);
    lv_label_set_text(detail_jardin_ui.battery, LV_SYMBOL_BATTERY_EMPTY " Bateria: --");
    lv_obj_set_style_text_color(detail_jardin_ui.battery, lv_color_hex(0x94A3B8), 0);
    lv_obj_set_style_text_font(detail_jardin_ui.battery, &lv_font_montserrat_24, 0);
    lv_obj_align(detail_jardin_ui.battery, LV_ALIGN_TOP_RIGHT, -30, 45);

    // Diferencia vs principal
    detail_jardin_ui.diff = lv_label_create(card_data);
    float diff_val = g_jardin.valid && g_weather.valid ? (g_jardin.temperature - g_weather.temp_outdoor) : 0;
    char diff_buf[48];
    snprintf(diff_buf, sizeof(diff_buf), "vs Principal: %+.1f°C (a la sombra)", diff_val);
    lv_label_set_text(detail_jardin_ui.diff, diff_buf);
    lv_obj_set_style_text_color(detail_jardin_ui.diff, lv_color_hex(0x93C5FD), 0);
    lv_obj_set_style_text_font(detail_jardin_ui.diff, &lv_font_montserrat_18, 0);
    lv_obj_align(detail_jardin_ui.diff, LV_ALIGN_BOTTOM_LEFT, 20, -10);

    // === CARD GRAFICA (abajo, a todo lo ancho) ===
    int chart_y = y_start + data_card_h + gap;
    int chart_h = SCREEN_HEIGHT - chart_y - 30;  // Dejar espacio para footer

    lv_obj_t *card_chart = lv_obj_create(scr_detail_jardin);
    lv_obj_set_pos(card_chart, gap, chart_y);
    lv_obj_set_size(card_chart, SCREEN_WIDTH - gap * 2, chart_h);
    lv_obj_set_style_bg_color(card_chart, card_dark, 0);
    lv_obj_set_style_radius(card_chart, 16, 0);
    lv_obj_set_style_border_width(card_chart, 1, 0);
    lv_obj_set_style_border_color(card_chart, border_dark, 0);
    lv_obj_set_style_pad_all(card_chart, 12, 0);
    lv_obj_clear_flag(card_chart, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *chart_title_lbl = lv_label_create(card_chart);
    lv_label_set_text(chart_title_lbl, "HISTORIAL 24H (pendiente API)");
    lv_obj_set_style_text_color(chart_title_lbl, text_muted, 0);
    lv_obj_set_style_text_font(chart_title_lbl, &lv_font_montserrat_12, 0);
    lv_obj_align(chart_title_lbl, LV_ALIGN_TOP_LEFT, 30, 0);

    // Chart a todo lo ancho
    detail_jardin_ui.chart = lv_chart_create(card_chart);
    lv_obj_set_size(detail_jardin_ui.chart, SCREEN_WIDTH - gap * 2 - 80, chart_h - 50);
    lv_obj_align(detail_jardin_ui.chart, LV_ALIGN_BOTTOM_RIGHT, -10, -18);
    lv_chart_set_type(detail_jardin_ui.chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(detail_jardin_ui.chart, 24);
    lv_chart_set_range(detail_jardin_ui.chart, LV_CHART_AXIS_PRIMARY_Y, 10, 35);
    lv_chart_set_div_line_count(detail_jardin_ui.chart, 5, 6);
    lv_obj_set_style_line_width(detail_jardin_ui.chart, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(detail_jardin_ui.chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(detail_jardin_ui.chart, subcard_dark, 0);
    lv_obj_set_style_border_width(detail_jardin_ui.chart, 0, 0);
    lv_obj_set_style_line_color(detail_jardin_ui.chart, grid_color, LV_PART_MAIN);
    lv_obj_set_style_line_opa(detail_jardin_ui.chart, LV_OPA_50, LV_PART_MAIN);

    detail_jardin_ui.ser_temp = lv_chart_add_series(detail_jardin_ui.chart, accent_green, LV_CHART_AXIS_PRIMARY_Y);
    for (int i = 0; i < 24; i++) {
        lv_chart_set_next_value(detail_jardin_ui.chart, detail_jardin_ui.ser_temp, 18 + (rand() % 12));
    }

    // Escala Y
    detail_jardin_ui.lbl_y_max = lv_label_create(card_chart);
    lv_label_set_text(detail_jardin_ui.lbl_y_max, "35°");
    lv_obj_set_style_text_color(detail_jardin_ui.lbl_y_max, text_muted, 0);
    lv_obj_set_style_text_font(detail_jardin_ui.lbl_y_max, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_jardin_ui.lbl_y_max, LV_ALIGN_TOP_LEFT, 5, 18);

    detail_jardin_ui.lbl_y_min = lv_label_create(card_chart);
    lv_label_set_text(detail_jardin_ui.lbl_y_min, "10°");
    lv_obj_set_style_text_color(detail_jardin_ui.lbl_y_min, text_muted, 0);
    lv_obj_set_style_text_font(detail_jardin_ui.lbl_y_min, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_jardin_ui.lbl_y_min, LV_ALIGN_BOTTOM_LEFT, 5, -22);

    // Escala X
    lv_obj_t *lbl_x_start = lv_label_create(card_chart);
    lv_label_set_text(lbl_x_start, "-24h");
    lv_obj_set_style_text_color(lbl_x_start, text_muted, 0);
    lv_obj_set_style_text_font(lbl_x_start, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_x_start, LV_ALIGN_BOTTOM_LEFT, 35, -5);

    lv_obj_t *lbl_x_end = lv_label_create(card_chart);
    lv_label_set_text(lbl_x_end, "Ahora");
    lv_obj_set_style_text_color(lbl_x_end, text_muted, 0);
    lv_obj_set_style_text_font(lbl_x_end, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl_x_end, LV_ALIGN_BOTTOM_RIGHT, -10, -5);

    // Footer
    lv_obj_t *footer = lv_label_create(scr_detail_jardin);
    lv_label_set_text(footer, LV_SYMBOL_LEFT " Swipe " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(footer, text_muted, 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -6);

    enableSwipeNavigation(scr_detail_jardin);
    Serial.println("[UI] Pantalla detalle Jardin creada (estilo dark)");
}

void updateDetailJardin() {
    extern RemoteSensorData g_jardin;
    extern WeatherData g_weather;

    if (!scr_detail_jardin) return;

    char buf[48];

    // Temperatura actual
    if (detail_jardin_ui.temp) {
        if (g_jardin.valid) {
            snprintf(buf, sizeof(buf), "%.1f°C", g_jardin.temperature);
        } else {
            snprintf(buf, sizeof(buf), "--.-°C");
        }
        lv_label_set_text(detail_jardin_ui.temp, buf);
    }

    // Humedad
    if (detail_jardin_ui.humidity) {
        if (g_jardin.valid) {
            snprintf(buf, sizeof(buf), "%.0f%%", g_jardin.humidity);
        } else {
            snprintf(buf, sizeof(buf), "--%");
        }
        lv_label_set_text(detail_jardin_ui.humidity, buf);
    }

    // Max/Min del día
    if (detail_jardin_ui.maxmin) {
        if (g_jardin.valid && g_jardin.temp_max > -100 && g_jardin.temp_min < 100) {
            snprintf(buf, sizeof(buf), LV_SYMBOL_UP " %.1f°  " LV_SYMBOL_DOWN " %.1f°",
                     g_jardin.temp_max, g_jardin.temp_min);
        } else {
            snprintf(buf, sizeof(buf), LV_SYMBOL_UP " --°  " LV_SYMBOL_DOWN " --°");
        }
        lv_label_set_text(detail_jardin_ui.maxmin, buf);
    }

    // Batería: OK o Baja (no porcentaje)
    if (detail_jardin_ui.battery) {
        if (g_jardin.valid) {
            if (g_jardin.battery > 50) {
                snprintf(buf, sizeof(buf), LV_SYMBOL_BATTERY_FULL " Bateria: OK");
                lv_obj_set_style_text_color(detail_jardin_ui.battery, lv_color_hex(0x6EE7B7), 0);
            } else {
                snprintf(buf, sizeof(buf), LV_SYMBOL_BATTERY_1 " Bateria: Baja!");
                lv_obj_set_style_text_color(detail_jardin_ui.battery, lv_color_hex(0xFCA5A5), 0);
            }
        } else {
            snprintf(buf, sizeof(buf), LV_SYMBOL_BATTERY_EMPTY " Bateria: --");
            lv_obj_set_style_text_color(detail_jardin_ui.battery, lv_color_hex(0x94A3B8), 0);
        }
        lv_label_set_text(detail_jardin_ui.battery, buf);
    }

    // Diferencia vs estación principal
    if (detail_jardin_ui.diff) {
        if (g_jardin.valid && g_weather.valid) {
            float diff_val = g_jardin.temperature - g_weather.temp_outdoor;
            snprintf(buf, sizeof(buf), "vs Principal: %+.1f°C", diff_val);
        } else {
            snprintf(buf, sizeof(buf), "vs Principal: --");
        }
        lv_label_set_text(detail_jardin_ui.diff, buf);
    }
}

/**
 * Pantalla detalle REMOTO (GW1100) - Plan 3b
 * Muestra valores actuales + gráficas de historial
 */

// UI elements for remote detail
static struct {
    lv_obj_t *temp;
    lv_obj_t *humidity;
    lv_obj_t *pressure;
    lv_obj_t *diff;
    // Charts
    lv_obj_t *chart_temp;
    lv_obj_t *chart_hum;
    lv_obj_t *chart_pres;
    lv_chart_series_t *ser_temp;
    lv_chart_series_t *ser_hum;
    lv_chart_series_t *ser_pres;
    lv_obj_t *stats_temp;
    lv_obj_t *stats_hum;
    lv_obj_t *stats_pres;
} detail_remoto_ui = {0};

void createDetailRemoto() {
    extern RemoteGatewayData g_remoto;
    extern lv_style_t style_card;
    extern bool darkMode;

    // Paleta de 3 tonos grises (sin tintes verdes/azules)
    lv_color_t bg_screen = darkMode ? lv_color_hex(0x0A0E12) : lv_color_hex(0xF8FAFC);   // Tono 1: casi negro
    lv_color_t bg_card = darkMode ? lv_color_hex(0x1A1F24) : lv_color_hex(0xFFFFFF);     // Tono 2: gris oscuro
    lv_color_t bg_subcard = darkMode ? lv_color_hex(0x2A3238) : lv_color_hex(0xF1F5F9);  // Tono 3: gris medio
    lv_color_t border_color = darkMode ? lv_color_hex(0x3A4248) : lv_color_hex(0xE2E8F0);

    scr_detail_remoto = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_detail_remoto, bg_screen, 0);  // Tono 1
    lv_obj_set_style_bg_opa(scr_detail_remoto, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr_detail_remoto, 0, 0);
    lv_obj_clear_flag(scr_detail_remoto, LV_OBJ_FLAG_SCROLLABLE);

    createDetailHeader(scr_detail_remoto, "ESTACION REMOTA", "Ecowitt GW1100", DETAIL_COLOR_REMOTO);

    // Layout estilo servidor: una card principal con grid 2x2 de sub-cards
    int gap = 12;
    int y_start = 78;

    // Colores exactos del servidor (Tailwind)
    lv_color_t color_temp = lv_color_hex(0xFCD34D);   // amber-300
    lv_color_t color_hum = lv_color_hex(0x67E8F9);    // cyan-300
    lv_color_t color_dew = lv_color_hex(0x6EE7B7);    // emerald-300
    lv_color_t color_pres = lv_color_hex(0xC4B5FD);   // violet-300

    // === CARD PRINCIPAL - Tono 2: gris oscuro ===
    lv_obj_t *main_card = lv_obj_create(scr_detail_remoto);
    lv_obj_set_size(main_card, SCREEN_WIDTH - gap * 2, SCREEN_HEIGHT - y_start - gap);
    lv_obj_set_pos(main_card, gap, y_start);
    lv_obj_set_style_bg_color(main_card, bg_card, 0);  // Tono 2
    lv_obj_set_style_bg_opa(main_card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(main_card, 16, 0);  // rounded-2xl
    lv_obj_set_style_border_width(main_card, 1, 0);
    lv_obj_set_style_border_color(main_card, border_color, 0);
    lv_obj_set_style_shadow_width(main_card, 30, 0);
    lv_obj_set_style_shadow_color(main_card, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(main_card, darkMode ? LV_OPA_40 : LV_OPA_10, 0);
    lv_obj_set_style_shadow_ofs_y(main_card, 8, 0);
    lv_obj_set_style_pad_all(main_card, 16, 0);
    lv_obj_clear_flag(main_card, LV_OBJ_FLAG_SCROLLABLE);

    // Título estilo servidor: card-title
    lv_obj_t *lbl_title = lv_label_create(main_card);
    lv_label_set_text(lbl_title, "LECTURAS EN VIVO");
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(lbl_title, darkMode ? lv_color_hex(0x94A3B8) : lv_color_hex(0x64748B), 0);
    lv_obj_align(lbl_title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Badge "en vivo" como el servidor
    lv_obj_t *badge = lv_obj_create(main_card);
    lv_obj_set_size(badge, 65, 22);
    lv_obj_align(badge, LV_ALIGN_TOP_RIGHT, 0, -2);
    lv_obj_set_style_bg_color(badge, lv_color_hex(0x34D399), 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_20, 0);
    lv_obj_set_style_radius(badge, 11, 0);
    lv_obj_set_style_border_width(badge, 1, 0);
    lv_obj_set_style_border_color(badge, lv_color_hex(0x34D399), 0);
    lv_obj_set_style_border_opa(badge, LV_OPA_50, 0);
    lv_obj_set_style_pad_all(badge, 0, 0);
    lv_obj_clear_flag(badge, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *badge_txt = lv_label_create(badge);
    lv_label_set_text(badge_txt, "EN VIVO");
    lv_obj_set_style_text_font(badge_txt, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(badge_txt, lv_color_hex(0x6EE7B7), 0);
    lv_obj_center(badge_txt);

    // === GRID 2x2 de sub-cards (como RemoteStationCard del servidor) ===
    int sc_w = (SCREEN_WIDTH - gap * 2 - 32 - 12) / 2;  // ~482px cada una
    int sc_h = 195;
    int grid_y = 30;

    // --- Sub-card 1: Temperatura (top-left) - Tono 3 ---
    lv_obj_t *sc_temp = lv_obj_create(main_card);
    lv_obj_set_size(sc_temp, sc_w, sc_h);
    lv_obj_set_pos(sc_temp, 0, grid_y);
    lv_obj_set_style_bg_color(sc_temp, bg_subcard, 0);  // Tono 3
    lv_obj_set_style_bg_opa(sc_temp, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(sc_temp, 10, 0);  // rounded-lg
    lv_obj_set_style_border_width(sc_temp, 0, 0);
    lv_obj_set_style_pad_all(sc_temp, 12, 0);
    lv_obj_clear_flag(sc_temp, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *temp_icon = lv_label_create(sc_temp);
    lv_label_set_text(temp_icon, WI_THERMOMETER);
    lv_obj_set_style_text_font(temp_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(temp_icon, color_temp, 0);
    lv_obj_align(temp_icon, LV_ALIGN_TOP_MID, 0, 0);

    detail_remoto_ui.temp = lv_label_create(sc_temp);
    lv_label_set_text(detail_remoto_ui.temp, "--.-°C");
    lv_obj_set_style_text_color(detail_remoto_ui.temp, color_temp, 0);
    lv_obj_set_style_text_font(detail_remoto_ui.temp, &lv_font_montserrat_28, 0);
    lv_obj_align(detail_remoto_ui.temp, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t *temp_lbl = lv_label_create(sc_temp);
    lv_label_set_text(temp_lbl, "Temperatura");
    lv_obj_set_style_text_font(temp_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(temp_lbl, darkMode ? lv_color_hex(0x94A3B8) : lv_color_hex(0x64748B), 0);
    lv_obj_align(temp_lbl, LV_ALIGN_TOP_MID, 0, 72);

    // Mini gráfica temperatura con cuadrícula punteada visible
    lv_color_t grid_color = darkMode ? lv_color_hex(0x5A6570) : lv_color_hex(0xA0A8B0);
    lv_color_t axis_color = darkMode ? lv_color_hex(0x8496A6) : lv_color_hex(0x64748B);

    detail_remoto_ui.chart_temp = lv_chart_create(sc_temp);
    lv_obj_set_size(detail_remoto_ui.chart_temp, sc_w - 60, 70);
    lv_obj_align(detail_remoto_ui.chart_temp, LV_ALIGN_BOTTOM_RIGHT, -8, -18);
    lv_chart_set_type(detail_remoto_ui.chart_temp, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(detail_remoto_ui.chart_temp, 36);
    lv_chart_set_range(detail_remoto_ui.chart_temp, LV_CHART_AXIS_PRIMARY_Y, 15, 30);
    lv_obj_set_style_line_width(detail_remoto_ui.chart_temp, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(detail_remoto_ui.chart_temp, 0, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(detail_remoto_ui.chart_temp, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(detail_remoto_ui.chart_temp, 0, 0);
    lv_chart_set_div_line_count(detail_remoto_ui.chart_temp, 3, 6);
    lv_obj_set_style_line_color(detail_remoto_ui.chart_temp, grid_color, LV_PART_MAIN);
    lv_obj_set_style_line_opa(detail_remoto_ui.chart_temp, LV_OPA_70, LV_PART_MAIN);
    lv_obj_set_style_line_dash_width(detail_remoto_ui.chart_temp, 4, LV_PART_MAIN);  // Punteada
    lv_obj_set_style_line_dash_gap(detail_remoto_ui.chart_temp, 4, LV_PART_MAIN);
    detail_remoto_ui.ser_temp = lv_chart_add_series(detail_remoto_ui.chart_temp, color_temp, LV_CHART_AXIS_PRIMARY_Y);

    // Etiquetas eje Y (temperatura)
    lv_obj_t *temp_max_lbl = lv_label_create(sc_temp);
    lv_label_set_text(temp_max_lbl, "30°");
    lv_obj_set_style_text_font(temp_max_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(temp_max_lbl, axis_color, 0);
    lv_obj_align_to(temp_max_lbl, detail_remoto_ui.chart_temp, LV_ALIGN_OUT_LEFT_TOP, -2, 0);

    lv_obj_t *temp_min_lbl = lv_label_create(sc_temp);
    lv_label_set_text(temp_min_lbl, "15°");
    lv_obj_set_style_text_font(temp_min_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(temp_min_lbl, axis_color, 0);
    lv_obj_align_to(temp_min_lbl, detail_remoto_ui.chart_temp, LV_ALIGN_OUT_LEFT_BOTTOM, -2, 0);

    // Etiquetas eje X (tiempo)
    lv_obj_t *temp_t1 = lv_label_create(sc_temp);
    lv_label_set_text(temp_t1, "-6h");
    lv_obj_set_style_text_font(temp_t1, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(temp_t1, axis_color, 0);
    lv_obj_align_to(temp_t1, detail_remoto_ui.chart_temp, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);

    lv_obj_t *temp_t2 = lv_label_create(sc_temp);
    lv_label_set_text(temp_t2, "Ahora");
    lv_obj_set_style_text_font(temp_t2, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(temp_t2, axis_color, 0);
    lv_obj_align_to(temp_t2, detail_remoto_ui.chart_temp, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 2);

    // --- Sub-card 2: Humedad (top-right) - Tono 3 ---
    lv_obj_t *sc_hum = lv_obj_create(main_card);
    lv_obj_set_size(sc_hum, sc_w, sc_h);
    lv_obj_set_pos(sc_hum, sc_w + 12, grid_y);
    lv_obj_set_style_bg_color(sc_hum, bg_subcard, 0);  // Tono 3
    lv_obj_set_style_bg_opa(sc_hum, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(sc_hum, 10, 0);
    lv_obj_set_style_border_width(sc_hum, 0, 0);
    lv_obj_set_style_pad_all(sc_hum, 12, 0);
    lv_obj_clear_flag(sc_hum, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *hum_icon = lv_label_create(sc_hum);
    lv_label_set_text(hum_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(hum_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(hum_icon, color_hum, 0);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_MID, 0, 0);

    detail_remoto_ui.humidity = lv_label_create(sc_hum);
    lv_label_set_text(detail_remoto_ui.humidity, "--%");
    lv_obj_set_style_text_color(detail_remoto_ui.humidity, color_hum, 0);
    lv_obj_set_style_text_font(detail_remoto_ui.humidity, &lv_font_montserrat_28, 0);
    lv_obj_align(detail_remoto_ui.humidity, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t *hum_lbl = lv_label_create(sc_hum);
    lv_label_set_text(hum_lbl, "Humedad");
    lv_obj_set_style_text_font(hum_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(hum_lbl, darkMode ? lv_color_hex(0x94A3B8) : lv_color_hex(0x64748B), 0);
    lv_obj_align(hum_lbl, LV_ALIGN_TOP_MID, 0, 72);

    detail_remoto_ui.chart_hum = lv_chart_create(sc_hum);
    lv_obj_set_size(detail_remoto_ui.chart_hum, sc_w - 60, 70);
    lv_obj_align(detail_remoto_ui.chart_hum, LV_ALIGN_BOTTOM_RIGHT, -8, -18);
    lv_chart_set_type(detail_remoto_ui.chart_hum, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(detail_remoto_ui.chart_hum, 36);
    lv_chart_set_range(detail_remoto_ui.chart_hum, LV_CHART_AXIS_PRIMARY_Y, 30, 100);
    lv_obj_set_style_line_width(detail_remoto_ui.chart_hum, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(detail_remoto_ui.chart_hum, 0, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(detail_remoto_ui.chart_hum, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(detail_remoto_ui.chart_hum, 0, 0);
    lv_chart_set_div_line_count(detail_remoto_ui.chart_hum, 3, 6);
    lv_obj_set_style_line_color(detail_remoto_ui.chart_hum, grid_color, LV_PART_MAIN);
    lv_obj_set_style_line_opa(detail_remoto_ui.chart_hum, LV_OPA_70, LV_PART_MAIN);
    lv_obj_set_style_line_dash_width(detail_remoto_ui.chart_hum, 4, LV_PART_MAIN);  // Punteada
    lv_obj_set_style_line_dash_gap(detail_remoto_ui.chart_hum, 4, LV_PART_MAIN);
    detail_remoto_ui.ser_hum = lv_chart_add_series(detail_remoto_ui.chart_hum, color_hum, LV_CHART_AXIS_PRIMARY_Y);

    // Etiquetas eje Y (humedad)
    lv_obj_t *hum_max_lbl = lv_label_create(sc_hum);
    lv_label_set_text(hum_max_lbl, "100%");
    lv_obj_set_style_text_font(hum_max_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(hum_max_lbl, axis_color, 0);
    lv_obj_align_to(hum_max_lbl, detail_remoto_ui.chart_hum, LV_ALIGN_OUT_LEFT_TOP, -2, 0);

    lv_obj_t *hum_min_lbl = lv_label_create(sc_hum);
    lv_label_set_text(hum_min_lbl, "30%");
    lv_obj_set_style_text_font(hum_min_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(hum_min_lbl, axis_color, 0);
    lv_obj_align_to(hum_min_lbl, detail_remoto_ui.chart_hum, LV_ALIGN_OUT_LEFT_BOTTOM, -2, 0);

    // Etiquetas eje X (tiempo)
    lv_obj_t *hum_t1 = lv_label_create(sc_hum);
    lv_label_set_text(hum_t1, "-6h");
    lv_obj_set_style_text_font(hum_t1, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(hum_t1, axis_color, 0);
    lv_obj_align_to(hum_t1, detail_remoto_ui.chart_hum, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);

    lv_obj_t *hum_t2 = lv_label_create(sc_hum);
    lv_label_set_text(hum_t2, "Ahora");
    lv_obj_set_style_text_font(hum_t2, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(hum_t2, axis_color, 0);
    lv_obj_align_to(hum_t2, detail_remoto_ui.chart_hum, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 2);

    // --- Sub-card 3: Punto de rocío + Sensación térmica (bottom-left) - Tono 3 ---
    int row2_y = grid_y + sc_h + 12;
    lv_obj_t *sc_derived = lv_obj_create(main_card);
    lv_obj_set_size(sc_derived, sc_w, sc_h);
    lv_obj_set_pos(sc_derived, 0, row2_y);
    lv_obj_set_style_bg_color(sc_derived, bg_subcard, 0);  // Tono 3
    lv_obj_set_style_bg_opa(sc_derived, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(sc_derived, 10, 0);
    lv_obj_set_style_border_width(sc_derived, 0, 0);
    lv_obj_set_style_pad_all(sc_derived, 12, 0);
    lv_obj_clear_flag(sc_derived, LV_OBJ_FLAG_SCROLLABLE);

    // Título de la card
    lv_obj_t *derived_title = lv_label_create(sc_derived);
    lv_label_set_text(derived_title, "DERIVADOS");
    lv_obj_set_style_text_font(derived_title, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(derived_title, darkMode ? lv_color_hex(0x94A3B8) : lv_color_hex(0x64748B), 0);
    lv_obj_align(derived_title, LV_ALIGN_TOP_MID, 0, 0);

    // Punto de rocío (izquierda)
    lv_obj_t *dew_icon = lv_label_create(sc_derived);
    lv_label_set_text(dew_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(dew_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(dew_icon, color_dew, 0);
    lv_obj_align(dew_icon, LV_ALIGN_TOP_LEFT, 30, 30);

    detail_remoto_ui.stats_temp = lv_label_create(sc_derived);  // Punto rocío
    lv_label_set_text(detail_remoto_ui.stats_temp, "--.-°C");
    lv_obj_set_style_text_color(detail_remoto_ui.stats_temp, color_dew, 0);
    lv_obj_set_style_text_font(detail_remoto_ui.stats_temp, &lv_font_montserrat_24, 0);
    lv_obj_align(detail_remoto_ui.stats_temp, LV_ALIGN_TOP_LEFT, 75, 35);

    lv_obj_t *dew_lbl = lv_label_create(sc_derived);
    lv_label_set_text(dew_lbl, "Punto de rocio");
    lv_obj_set_style_text_font(dew_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(dew_lbl, darkMode ? lv_color_hex(0x94A3B8) : lv_color_hex(0x64748B), 0);
    lv_obj_align(dew_lbl, LV_ALIGN_TOP_LEFT, 30, 70);

    // Sensación térmica (derecha)
    lv_obj_t *feels_icon = lv_label_create(sc_derived);
    lv_label_set_text(feels_icon, WI_THERMOMETER);
    lv_obj_set_style_text_font(feels_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(feels_icon, lv_color_hex(0xFB923C), 0);  // orange-400
    lv_obj_align(feels_icon, LV_ALIGN_TOP_RIGHT, -180, 30);

    detail_remoto_ui.stats_hum = lv_label_create(sc_derived);  // Sensación térmica
    lv_label_set_text(detail_remoto_ui.stats_hum, "--.-°C");
    lv_obj_set_style_text_color(detail_remoto_ui.stats_hum, lv_color_hex(0xFB923C), 0);
    lv_obj_set_style_text_font(detail_remoto_ui.stats_hum, &lv_font_montserrat_24, 0);
    lv_obj_align(detail_remoto_ui.stats_hum, LV_ALIGN_TOP_RIGHT, -85, 35);

    lv_obj_t *feels_lbl = lv_label_create(sc_derived);
    lv_label_set_text(feels_lbl, "Sensacion termica");
    lv_obj_set_style_text_font(feels_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(feels_lbl, darkMode ? lv_color_hex(0x94A3B8) : lv_color_hex(0x64748B), 0);
    lv_obj_align(feels_lbl, LV_ALIGN_TOP_RIGHT, -65, 70);

    // Nota explicativa
    lv_obj_t *note_lbl = lv_label_create(sc_derived);
    lv_label_set_text(note_lbl, "Calculados a partir de temperatura y humedad");
    lv_obj_set_style_text_font(note_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(note_lbl, darkMode ? lv_color_hex(0x64748B) : lv_color_hex(0x94A3B8), 0);
    lv_obj_align(note_lbl, LV_ALIGN_BOTTOM_MID, 0, -10);

    // --- Sub-card 4: Presión (bottom-right) ---
    // --- Sub-card 4: Presión (bottom-right) - Tono 3 ---
    lv_obj_t *sc_pres = lv_obj_create(main_card);
    lv_obj_set_size(sc_pres, sc_w, sc_h);
    lv_obj_set_pos(sc_pres, sc_w + 12, row2_y);
    lv_obj_set_style_bg_color(sc_pres, bg_subcard, 0);  // Tono 3
    lv_obj_set_style_bg_opa(sc_pres, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(sc_pres, 10, 0);
    lv_obj_set_style_border_width(sc_pres, 0, 0);
    lv_obj_set_style_pad_all(sc_pres, 12, 0);
    lv_obj_clear_flag(sc_pres, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *pres_icon = lv_label_create(sc_pres);
    lv_label_set_text(pres_icon, WI_BAROMETER);
    lv_obj_set_style_text_font(pres_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(pres_icon, color_pres, 0);
    lv_obj_align(pres_icon, LV_ALIGN_TOP_MID, 0, 0);

    detail_remoto_ui.pressure = lv_label_create(sc_pres);
    lv_label_set_text(detail_remoto_ui.pressure, "---- hPa");
    lv_obj_set_style_text_color(detail_remoto_ui.pressure, color_pres, 0);
    lv_obj_set_style_text_font(detail_remoto_ui.pressure, &lv_font_montserrat_24, 0);
    lv_obj_align(detail_remoto_ui.pressure, LV_ALIGN_TOP_MID, 0, 40);

    lv_obj_t *pres_lbl = lv_label_create(sc_pres);
    lv_label_set_text(pres_lbl, "Presion");
    lv_obj_set_style_text_font(pres_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(pres_lbl, darkMode ? lv_color_hex(0x94A3B8) : lv_color_hex(0x64748B), 0);
    lv_obj_align(pres_lbl, LV_ALIGN_TOP_MID, 0, 68);

    detail_remoto_ui.chart_pres = lv_chart_create(sc_pres);
    lv_obj_set_size(detail_remoto_ui.chart_pres, sc_w - 60, 70);
    lv_obj_align(detail_remoto_ui.chart_pres, LV_ALIGN_BOTTOM_RIGHT, -8, -18);
    lv_chart_set_type(detail_remoto_ui.chart_pres, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(detail_remoto_ui.chart_pres, 36);
    lv_chart_set_range(detail_remoto_ui.chart_pres, LV_CHART_AXIS_PRIMARY_Y, 1000, 1025);
    lv_obj_set_style_line_width(detail_remoto_ui.chart_pres, 2, LV_PART_ITEMS);
    lv_obj_set_style_size(detail_remoto_ui.chart_pres, 0, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(detail_remoto_ui.chart_pres, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(detail_remoto_ui.chart_pres, 0, 0);
    lv_chart_set_div_line_count(detail_remoto_ui.chart_pres, 3, 6);
    lv_obj_set_style_line_color(detail_remoto_ui.chart_pres, grid_color, LV_PART_MAIN);
    lv_obj_set_style_line_opa(detail_remoto_ui.chart_pres, LV_OPA_70, LV_PART_MAIN);
    lv_obj_set_style_line_dash_width(detail_remoto_ui.chart_pres, 4, LV_PART_MAIN);  // Punteada
    lv_obj_set_style_line_dash_gap(detail_remoto_ui.chart_pres, 4, LV_PART_MAIN);
    detail_remoto_ui.ser_pres = lv_chart_add_series(detail_remoto_ui.chart_pres, color_pres, LV_CHART_AXIS_PRIMARY_Y);

    // Etiquetas eje Y (presión)
    lv_obj_t *pres_max_lbl = lv_label_create(sc_pres);
    lv_label_set_text(pres_max_lbl, "1025");
    lv_obj_set_style_text_font(pres_max_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(pres_max_lbl, axis_color, 0);
    lv_obj_align_to(pres_max_lbl, detail_remoto_ui.chart_pres, LV_ALIGN_OUT_LEFT_TOP, -2, 0);

    lv_obj_t *pres_min_lbl = lv_label_create(sc_pres);
    lv_label_set_text(pres_min_lbl, "1000");
    lv_obj_set_style_text_font(pres_min_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(pres_min_lbl, axis_color, 0);
    lv_obj_align_to(pres_min_lbl, detail_remoto_ui.chart_pres, LV_ALIGN_OUT_LEFT_BOTTOM, -2, 0);

    // Etiquetas eje X (tiempo)
    lv_obj_t *pres_t1 = lv_label_create(sc_pres);
    lv_label_set_text(pres_t1, "-6h");
    lv_obj_set_style_text_font(pres_t1, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(pres_t1, axis_color, 0);
    lv_obj_align_to(pres_t1, detail_remoto_ui.chart_pres, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 2);

    lv_obj_t *pres_t2 = lv_label_create(sc_pres);
    lv_label_set_text(pres_t2, "Ahora");
    lv_obj_set_style_text_font(pres_t2, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(pres_t2, axis_color, 0);
    lv_obj_align_to(pres_t2, detail_remoto_ui.chart_pres, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 2);

    // Footer con comparación vs principal
    detail_remoto_ui.diff = lv_label_create(main_card);
    lv_label_set_text(detail_remoto_ui.diff, "vs Principal: --");
    lv_obj_set_style_text_color(detail_remoto_ui.diff, darkMode ? lv_color_hex(0x94A3B8) : lv_color_hex(0x64748B), 0);
    lv_obj_set_style_text_font(detail_remoto_ui.diff, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_remoto_ui.diff, LV_ALIGN_BOTTOM_MID, 0, -5);

    // Stats no usados
    detail_remoto_ui.stats_pres = nullptr;

    // Datos placeholder con tendencia realista
    for (int i = 0; i < 36; i++) {
        int temp = 22 + (int)(3 * sin(i * 0.18));
        int hum = 60 + (int)(12 * cos(i * 0.15));
        int pres = 1012 + (int)(5 * sin(i * 0.1));
        lv_chart_set_next_value(detail_remoto_ui.chart_temp, detail_remoto_ui.ser_temp, temp);
        lv_chart_set_next_value(detail_remoto_ui.chart_hum, detail_remoto_ui.ser_hum, hum);
        lv_chart_set_next_value(detail_remoto_ui.chart_pres, detail_remoto_ui.ser_pres, pres);
    }

    enableSwipeNavigation(scr_detail_remoto);
    Serial.println("[UI] Pantalla detalle Remoto creada (estilo servidor grid 2x2)");
}

void updateDetailRemoto() {
    extern RemoteGatewayData g_remoto;
    extern WeatherData g_weather;
    char buf[128];

    if (!g_remoto.valid) {
        if (detail_remoto_ui.temp) {
            lv_label_set_text(detail_remoto_ui.temp, "--.-°C");
        }
        if (detail_remoto_ui.humidity) {
            lv_label_set_text(detail_remoto_ui.humidity, "--%");
        }
        if (detail_remoto_ui.pressure) {
            lv_label_set_text(detail_remoto_ui.pressure, "---- hPa");
        }
        if (detail_remoto_ui.stats_temp) {
            lv_label_set_text(detail_remoto_ui.stats_temp, "--.-°C");
        }
        if (detail_remoto_ui.diff) {
            lv_label_set_text(detail_remoto_ui.diff, "Sin datos de estacion remota");
        }
        return;
    }

    // Temperatura
    if (detail_remoto_ui.temp) {
        snprintf(buf, sizeof(buf), "%.1f°C", g_remoto.temperature);
        lv_label_set_text(detail_remoto_ui.temp, buf);
    }

    // Humedad
    if (detail_remoto_ui.humidity) {
        snprintf(buf, sizeof(buf), "%.0f%%", g_remoto.humidity);
        lv_label_set_text(detail_remoto_ui.humidity, buf);
    }

    // Punto de rocío (Magnus formula)
    float t = g_remoto.temperature;
    float h = g_remoto.humidity;
    if (detail_remoto_ui.stats_temp) {
        float a = 17.27;
        float b = 237.7;
        float alpha = (a * t) / (b + t) + log(h / 100.0);
        float dew = (b * alpha) / (a - alpha);
        snprintf(buf, sizeof(buf), "%.1f°C", dew);
        lv_label_set_text(detail_remoto_ui.stats_temp, buf);
    }

    // Sensación térmica (Heat Index simplificado para T>20°C y H>40%)
    if (detail_remoto_ui.stats_hum) {
        float feels;
        if (t >= 20.0 && h >= 40.0) {
            // Heat Index formula (Rothfusz regression)
            float tf = t * 9.0/5.0 + 32.0;  // Convert to Fahrenheit
            float hi = -42.379 + 2.04901523*tf + 10.14333127*h
                       - 0.22475541*tf*h - 0.00683783*tf*tf
                       - 0.05481717*h*h + 0.00122874*tf*tf*h
                       + 0.00085282*tf*h*h - 0.00000199*tf*tf*h*h;
            feels = (hi - 32.0) * 5.0/9.0;  // Convert back to Celsius
        } else if (t <= 10.0) {
            // Wind chill (simplified - no wind data available)
            feels = t;  // Without wind, feels like actual temp
        } else {
            feels = t;  // Comfort range
        }
        snprintf(buf, sizeof(buf), "%.1f°C", feels);
        lv_label_set_text(detail_remoto_ui.stats_hum, buf);
    }

    // Presion
    if (detail_remoto_ui.pressure) {
        snprintf(buf, sizeof(buf), "%.0f hPa", g_remoto.pressure);
        lv_label_set_text(detail_remoto_ui.pressure, buf);
    }

    // Diferencia vs principal
    if (detail_remoto_ui.diff && g_weather.valid) {
        float temp_diff = g_remoto.temperature - g_weather.temp_outdoor;
        float pres_diff = g_remoto.pressure - g_weather.pressure_rel;
        snprintf(buf, sizeof(buf), "vs Principal: %+.1f°C  |  Presion: %+.0f hPa",
                 temp_diff, pres_diff);
        lv_label_set_text(detail_remoto_ui.diff, buf);
    }

    // TODO: Cargar historial real del servidor cuando esté disponible
}

// ============================================================================
// Inicialización de navegación
// ============================================================================

void initNavigation() {
    extern lv_obj_t *scr_dashboard;
    extern lv_obj_t *btn_settings;

    // Habilitar swipe en dashboard
    enableSwipeNavigation(scr_dashboard);

    // Crear indicador de página
    createPageIndicator(scr_dashboard);

    // Settings button callback
    if (btn_settings) {
        lv_obj_add_event_cb(btn_settings, [](lv_event_t *e) {
            navigateToScreen(SCREEN_SETTINGS);
            updatePageIndicator();
        }, LV_EVENT_CLICKED, NULL);
    }

    Serial.println("[NAV] Sistema de navegación inicializado");
}

#endif // UI_NAVIGATION_H
