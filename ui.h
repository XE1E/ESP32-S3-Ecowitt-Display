/**
 * ui.h - LVGL User Interface
 *
 * Glassmorphism design with tabview navigation
 */

#ifndef UI_H
#define UI_H

#include <Arduino.h>
#include <lvgl.h>
#include <time.h>
#include "config.h"
#include "backgrounds.h"
#include "data_watchdog.h"

// Seleccionar version de UI: comentar para usar UI original (glassmorphism oscuro)
// #define USE_UI_V2  // Usar diseño V2 claro estilo widget
#define USE_UI_V3  // Usar diseño V3 exacto de imagen referencia

// Forward declarations
void updateHistoryUI();
void switchToMainScreen();
void switchToTabsScreen();
void createHistoryTab(lv_obj_t *parent);
void formatTime24h(char *buffer, size_t size);
void formatDateLong(char *buffer, size_t size);

// V1 (original) UI functions - only when V2/V3 not active
#if !defined(USE_UI_V2) && !defined(USE_UI_V3)
void updateMainScreenTime();
void updateMainScreenWeather(WeatherData *weather);
void updateMainScreenForecast(ForecastData *forecast);
lv_obj_t* getMainScreen();
void setAutoBackground(lv_obj_t *target);
void setThemeBackground(lv_obj_t *target, int themeIndex);
void createMainScreen();
void showMainScreen();
void createMainScreenContent(lv_obj_t *parent);
#endif
#ifdef USE_UI_V2
void createMainScreenV2Content(lv_obj_t *parent);
void updateMainScreenV2Time();
void updateMainScreenV2Weather(WeatherData *weather);
void updateMainScreenV2Forecast(ForecastData *forecast);
void updateConnectionStatusV2();
#endif

#ifdef USE_UI_V3
void createMainScreenV3Content(lv_obj_t *parent);
void updateMainScreenV3Time();
void updateMainScreenV3Weather(WeatherData *weather);
void updateMainScreenV3Forecast(ForecastData *forecast);
void updateConnectionStatusV3();
void showFixedLayers();
void hideFixedLayers();
void createFixedLayers();
#endif

// Screen mode
enum ScreenMode {
    SCREEN_MODE_MAIN = 0,
    SCREEN_MODE_TABS
};
static ScreenMode currentScreenMode = SCREEN_MODE_MAIN;

// ============================================================================
// UI Elements - Hidden/Visible Pattern
// ============================================================================

// Single base screen (all content lives here)
static lv_obj_t *base_screen = nullptr;

// Main dashboard container (shown on main view)
static lv_obj_t *container_main = nullptr;

// Tabs container (shown on tabs view)
static lv_obj_t *container_tabs = nullptr;

// Legacy reference (points to container_tabs for compatibility)
static lv_obj_t *screen_main = nullptr;
static lv_obj_t *tabview = nullptr;
static lv_obj_t *tab_dashboard = nullptr;
static lv_obj_t *tab_trends = nullptr;
static lv_obj_t *tab_history = nullptr;
static lv_obj_t *tab_settings = nullptr;

// Dashboard elements
static lv_obj_t *lbl_time = nullptr;
static lv_obj_t *lbl_date = nullptr;
static lv_obj_t *lbl_temp_outdoor = nullptr;
static lv_obj_t *lbl_condition = nullptr;
static lv_obj_t *lbl_humidity = nullptr;
static lv_obj_t *lbl_wind = nullptr;
static lv_obj_t *lbl_pressure = nullptr;
static lv_obj_t *panel_indoor = nullptr;
static lv_obj_t *panel_outdoor = nullptr;
static lv_obj_t *panel_moon = nullptr;

// Splash screen
static lv_obj_t *splash_screen = nullptr;
static lv_obj_t *splash_status = nullptr;

// Styles
static lv_style_t style_panel;
static lv_style_t style_title;
static lv_style_t style_value;
static lv_style_t style_label;
static lv_style_t style_time;

// ============================================================================
// Style Initialization
// ============================================================================

void initStyles() {
    // Panel style (glassmorphism)
    lv_style_init(&style_panel);
    lv_style_set_bg_color(&style_panel, lv_color_white());
    lv_style_set_bg_opa(&style_panel, LV_OPA_20);
    lv_style_set_radius(&style_panel, 20);
    lv_style_set_border_width(&style_panel, 1);
    lv_style_set_border_color(&style_panel, lv_color_white());
    lv_style_set_border_opa(&style_panel, LV_OPA_30);
    lv_style_set_pad_all(&style_panel, 15);

    // Title style
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, lv_color_hex(0x1a1a2e));
    lv_style_set_text_font(&style_title, &lv_font_montserrat_24);

    // Value style (large numbers)
    lv_style_init(&style_value);
    lv_style_set_text_color(&style_value, lv_color_hex(0x1a1a2e));
    lv_style_set_text_font(&style_value, &lv_font_montserrat_48);

    // Label style
    lv_style_init(&style_label);
    lv_style_set_text_color(&style_label, lv_color_hex(0x666666));
    lv_style_set_text_font(&style_label, &lv_font_montserrat_16);

    // Time style (extra large)
    lv_style_init(&style_time);
    lv_style_set_text_color(&style_time, lv_color_hex(0x1a1a2e));
    // Note: Would need custom font for 72px
    lv_style_set_text_font(&style_time, &lv_font_montserrat_48);
}

// ============================================================================
// Create Panel (Glassmorphism card)
// ============================================================================

lv_obj_t* createPanel(lv_obj_t *parent, int x, int y, int w, int h) {
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_pos(panel, x, y);
    lv_obj_set_size(panel, w, h);
    lv_obj_add_style(panel, &style_panel, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    return panel;
}

// ============================================================================
// Splash Screen
// ============================================================================

void showSplashScreen() {
    splash_screen = lv_obj_create(lv_scr_act());
    lv_obj_set_size(splash_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(splash_screen, lv_color_hex(0x87CEEB), 0);
    lv_obj_clear_flag(splash_screen, LV_OBJ_FLAG_SCROLLABLE);

    // Title
    lv_obj_t *title = lv_label_create(splash_screen);
    lv_label_set_text(title, "Estacion Clima");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -50);

    // Status
    splash_status = lv_label_create(splash_screen);
    lv_label_set_text(splash_status, "Iniciando...");
    lv_obj_set_style_text_font(splash_status, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(splash_status, lv_color_white(), 0);
    lv_obj_align(splash_status, LV_ALIGN_CENTER, 0, 50);

    lv_refr_now(NULL);  // Force immediate refresh
}

void updateSplashStatus(const char *status) {
    if (splash_status) {
        lv_label_set_text(splash_status, status);
        lv_refr_now(NULL);  // Force immediate refresh
    }
}

void hideSplashScreen() {
    if (splash_screen) {
        lv_obj_del(splash_screen);
        splash_screen = nullptr;
        splash_status = nullptr;
    }
}

// ============================================================================
// Create Dashboard Tab
// ============================================================================

void createDashboard(lv_obj_t *parent) {
    // Time display (top center)
    lv_obj_t *time_panel = createPanel(parent, 362, 10, 300, 120);
    lv_obj_set_style_bg_opa(time_panel, LV_OPA_30, 0);

    lbl_time = lv_label_create(time_panel);
    lv_label_set_text(lbl_time, "00:00");
    lv_obj_add_style(lbl_time, &style_time, 0);
    lv_obj_align(lbl_time, LV_ALIGN_TOP_MID, 0, 5);

    lbl_date = lv_label_create(time_panel);
    lv_label_set_text(lbl_date, "---");
    lv_obj_add_style(lbl_date, &style_label, 0);
    lv_obj_align(lbl_date, LV_ALIGN_BOTTOM_MID, 0, -5);

    // Indoor panel (left)
    panel_indoor = createPanel(parent, 20, 150, 300, 200);

    lv_obj_t *indoor_title = lv_label_create(panel_indoor);
    lv_label_set_text(indoor_title, "Interior");
    lv_obj_add_style(indoor_title, &style_title, 0);
    lv_obj_align(indoor_title, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t *indoor_temp = lv_label_create(panel_indoor);
    lv_label_set_text(indoor_temp, "--.-");
    lv_obj_add_style(indoor_temp, &style_value, 0);
    lv_obj_align(indoor_temp, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *indoor_unit = lv_label_create(panel_indoor);
    lv_label_set_text(indoor_unit, "C");
    lv_obj_add_style(indoor_unit, &style_label, 0);
    lv_obj_align(indoor_unit, LV_ALIGN_CENTER, 60, -10);

    // Outdoor panel (center)
    panel_outdoor = createPanel(parent, 340, 150, 340, 300);

    lv_obj_t *outdoor_title = lv_label_create(panel_outdoor);
    lv_label_set_text(outdoor_title, "Exterior");
    lv_obj_add_style(outdoor_title, &style_title, 0);
    lv_obj_align(outdoor_title, LV_ALIGN_TOP_LEFT, 0, 0);

    lbl_temp_outdoor = lv_label_create(panel_outdoor);
    lv_label_set_text(lbl_temp_outdoor, "--.-");
    lv_obj_add_style(lbl_temp_outdoor, &style_value, 0);
    lv_obj_align(lbl_temp_outdoor, LV_ALIGN_TOP_MID, 0, 50);

    lbl_condition = lv_label_create(panel_outdoor);
    lv_label_set_text(lbl_condition, "---");
    lv_obj_add_style(lbl_condition, &style_label, 0);
    lv_obj_align(lbl_condition, LV_ALIGN_CENTER, 0, 20);

    // Weather details row
    lbl_humidity = lv_label_create(panel_outdoor);
    lv_label_set_text(lbl_humidity, "Humedad: --%");
    lv_obj_add_style(lbl_humidity, &style_label, 0);
    lv_obj_align(lbl_humidity, LV_ALIGN_BOTTOM_LEFT, 10, -40);

    lbl_wind = lv_label_create(panel_outdoor);
    lv_label_set_text(lbl_wind, "Viento: -- km/h");
    lv_obj_add_style(lbl_wind, &style_label, 0);
    lv_obj_align(lbl_wind, LV_ALIGN_BOTTOM_LEFT, 10, -15);

    lbl_pressure = lv_label_create(panel_outdoor);
    lv_label_set_text(lbl_pressure, "Presion: -- hPa");
    lv_obj_add_style(lbl_pressure, &style_label, 0);
    lv_obj_align(lbl_pressure, LV_ALIGN_BOTTOM_RIGHT, -10, -15);

    // Moon panel (right)
    panel_moon = createPanel(parent, 700, 150, 300, 200);

    lv_obj_t *moon_title = lv_label_create(panel_moon);
    lv_label_set_text(moon_title, "Luna");
    lv_obj_add_style(moon_title, &style_title, 0);
    lv_obj_align(moon_title, LV_ALIGN_TOP_LEFT, 0, 0);

    // Moon visualization would go here (custom canvas drawing)
}

// ============================================================================
// Create Trends Tab
// ============================================================================

// Labels for trends tab (hourly forecast)
static lv_obj_t *hourly_container = nullptr;
static lv_obj_t *hourly_labels[12];      // Show 12 hours
static lv_obj_t *hourly_temps[12];
static lv_obj_t *hourly_icons[12];
static lv_obj_t *hourly_rain[12];

// Labels for daily forecast
static lv_obj_t *daily_container = nullptr;
static lv_obj_t *daily_labels[MAX_FORECAST_DAYS];
static lv_obj_t *daily_max[MAX_FORECAST_DAYS];
static lv_obj_t *daily_min[MAX_FORECAST_DAYS];
static lv_obj_t *daily_icons[MAX_FORECAST_DAYS];
static lv_obj_t *daily_rain_chance[MAX_FORECAST_DAYS];

void createTrends(lv_obj_t *parent) {
    // Title
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "Tendencias");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_pos(title, 80, 10);

    // === Hourly Forecast Section ===
    lv_obj_t *hourly_title = lv_label_create(parent);
    lv_label_set_text(hourly_title, "Proximo 12 horas");
    lv_obj_add_style(hourly_title, &style_label, 0);
    lv_obj_set_pos(hourly_title, 20, 50);

    // Scrollable container for hourly cards
    hourly_container = lv_obj_create(parent);
    lv_obj_set_pos(hourly_container, 20, 75);
    lv_obj_set_size(hourly_container, SCREEN_WIDTH - 40, 150);
    lv_obj_set_style_bg_opa(hourly_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hourly_container, 0, 0);
    lv_obj_set_style_pad_all(hourly_container, 5, 0);
    lv_obj_set_flex_flow(hourly_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(hourly_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scroll_dir(hourly_container, LV_DIR_HOR);
    lv_obj_set_scroll_snap_x(hourly_container, LV_SCROLL_SNAP_START);

    // Create hourly cards
    for (int i = 0; i < 12; i++) {
        lv_obj_t *card = lv_obj_create(hourly_container);
        lv_obj_set_size(card, 75, 130);
        lv_obj_add_style(card, &style_panel, 0);
        lv_obj_set_style_bg_opa(card, LV_OPA_40, 0);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_pad_all(card, 8, 0);

        // Hour label (e.g., "14:00")
        hourly_labels[i] = lv_label_create(card);
        lv_label_set_text(hourly_labels[i], "--:--");
        lv_obj_set_style_text_font(hourly_labels[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(hourly_labels[i], lv_color_hex(0xaaaaaa), 0);
        lv_obj_align(hourly_labels[i], LV_ALIGN_TOP_MID, 0, 0);

        // Weather icon (using symbol as placeholder)
        hourly_icons[i] = lv_label_create(card);
        lv_label_set_text(hourly_icons[i], LV_SYMBOL_IMAGE);
        lv_obj_set_style_text_font(hourly_icons[i], &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(hourly_icons[i], lv_color_hex(0xffd700), 0);
        lv_obj_align(hourly_icons[i], LV_ALIGN_CENTER, 0, -10);

        // Temperature
        hourly_temps[i] = lv_label_create(card);
        lv_label_set_text(hourly_temps[i], "--");
        lv_obj_set_style_text_font(hourly_temps[i], &lv_font_montserrat_18, 0);
        lv_obj_set_style_text_color(hourly_temps[i], lv_color_white(), 0);
        lv_obj_align(hourly_temps[i], LV_ALIGN_CENTER, 0, 20);

        // Rain chance
        hourly_rain[i] = lv_label_create(card);
        lv_label_set_text(hourly_rain[i], "");
        lv_obj_set_style_text_font(hourly_rain[i], &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(hourly_rain[i], lv_color_hex(0x64b5f6), 0);
        lv_obj_align(hourly_rain[i], LV_ALIGN_BOTTOM_MID, 0, 0);
    }

    // === Daily Forecast Section ===
    lv_obj_t *daily_title = lv_label_create(parent);
    lv_label_set_text(daily_title, "Proximos dias");
    lv_obj_add_style(daily_title, &style_label, 0);
    lv_obj_set_pos(daily_title, 20, 235);

    // Container for daily cards
    daily_container = lv_obj_create(parent);
    lv_obj_set_pos(daily_container, 20, 260);
    lv_obj_set_size(daily_container, SCREEN_WIDTH - 40, 180);
    lv_obj_set_style_bg_opa(daily_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(daily_container, 0, 0);
    lv_obj_set_style_pad_all(daily_container, 5, 0);
    lv_obj_set_flex_flow(daily_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(daily_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(daily_container, LV_OBJ_FLAG_SCROLLABLE);

    // Create daily cards
    for (int i = 0; i < MAX_FORECAST_DAYS; i++) {
        lv_obj_t *card = lv_obj_create(daily_container);
        lv_obj_set_size(card, 140, 160);
        lv_obj_add_style(card, &style_panel, 0);
        lv_obj_set_style_bg_opa(card, LV_OPA_40, 0);
        lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_pad_all(card, 10, 0);

        // Day label (e.g., "Lunes")
        daily_labels[i] = lv_label_create(card);
        lv_label_set_text(daily_labels[i], "---");
        lv_obj_set_style_text_font(daily_labels[i], &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(daily_labels[i], lv_color_white(), 0);
        lv_obj_align(daily_labels[i], LV_ALIGN_TOP_MID, 0, 0);

        // Weather icon
        daily_icons[i] = lv_label_create(card);
        lv_label_set_text(daily_icons[i], LV_SYMBOL_IMAGE);
        lv_obj_set_style_text_font(daily_icons[i], &lv_font_montserrat_24, 0);
        lv_obj_set_style_text_color(daily_icons[i], lv_color_hex(0xffd700), 0);
        lv_obj_align(daily_icons[i], LV_ALIGN_CENTER, 0, -15);

        // Max temp
        daily_max[i] = lv_label_create(card);
        lv_label_set_text(daily_max[i], "--");
        lv_obj_set_style_text_font(daily_max[i], &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(daily_max[i], lv_color_hex(0xff6b6b), 0);
        lv_obj_align(daily_max[i], LV_ALIGN_CENTER, -20, 25);

        // Min temp
        daily_min[i] = lv_label_create(card);
        lv_label_set_text(daily_min[i], "--");
        lv_obj_set_style_text_font(daily_min[i], &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_color(daily_min[i], lv_color_hex(0x64b5f6), 0);
        lv_obj_align(daily_min[i], LV_ALIGN_CENTER, 20, 25);

        // Rain chance
        daily_rain_chance[i] = lv_label_create(card);
        lv_label_set_text(daily_rain_chance[i], "");
        lv_obj_set_style_text_font(daily_rain_chance[i], &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(daily_rain_chance[i], lv_color_hex(0x64b5f6), 0);
        lv_obj_align(daily_rain_chance[i], LV_ALIGN_BOTTOM_MID, 0, 0);
    }
}

// ============================================================================
// Create Settings Tab
// ============================================================================

void createSettings(lv_obj_t *parent) {
    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "Ajustes");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 20, 20);

    // === Left Column ===

    // WiFi configuration button
    lv_obj_t *btn_wifi = lv_btn_create(parent);
    lv_obj_set_pos(btn_wifi, 20, 70);
    lv_obj_set_size(btn_wifi, 280, 50);
    lv_obj_add_style(btn_wifi, &style_panel, 0);

    lv_obj_t *btn_label = lv_label_create(btn_wifi);
    lv_label_set_text(btn_label, LV_SYMBOL_WIFI " Configurar WiFi");
    lv_obj_center(btn_label);

    // API Key configuration button
    lv_obj_t *btn_api = lv_btn_create(parent);
    lv_obj_set_pos(btn_api, 20, 135);
    lv_obj_set_size(btn_api, 280, 50);
    lv_obj_add_style(btn_api, &style_panel, 0);

    lv_obj_t *api_label = lv_label_create(btn_api);
    lv_label_set_text(api_label, LV_SYMBOL_EDIT " API Key");
    lv_obj_center(api_label);

    // Location configuration button
    lv_obj_t *btn_loc = lv_btn_create(parent);
    lv_obj_set_pos(btn_loc, 20, 200);
    lv_obj_set_size(btn_loc, 280, 50);
    lv_obj_add_style(btn_loc, &style_panel, 0);

    lv_obj_t *loc_label = lv_label_create(btn_loc);
    lv_label_set_text(loc_label, LV_SYMBOL_GPS " Ubicacion");
    lv_obj_center(loc_label);

    // Factory reset button
    lv_obj_t *btn_reset = lv_btn_create(parent);
    lv_obj_set_pos(btn_reset, 20, 280);
    lv_obj_set_size(btn_reset, 280, 50);
    lv_obj_set_style_bg_color(btn_reset, lv_color_hex(0xc62828), 0);
    lv_obj_add_event_cb(btn_reset, [](lv_event_t *e) {
        // TODO: Show confirmation dialog
        Serial.println("Factory reset requested");
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *reset_label = lv_label_create(btn_reset);
    lv_label_set_text(reset_label, LV_SYMBOL_TRASH " Restablecer");
    lv_obj_set_style_text_color(reset_label, lv_color_white(), 0);
    lv_obj_center(reset_label);

    // === Right Column ===

    // Brightness
    lv_obj_t *lbl_bright = lv_label_create(parent);
    lv_label_set_text(lbl_bright, "Brillo");
    lv_obj_add_style(lbl_bright, &style_label, 0);
    lv_obj_set_pos(lbl_bright, 350, 70);

    lv_obj_t *slider = lv_slider_create(parent);
    lv_obj_set_pos(slider, 350, 100);
    lv_obj_set_size(slider, 280, 15);
    lv_slider_set_range(slider, 10, 100);  // IO Expander uses 0-100
    lv_slider_set_value(slider, appConfig.display.brightness, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, [](lv_event_t *e) {
        lv_obj_t *s = lv_event_get_target(e);
        appConfig.display.brightness = lv_slider_get_value(s);
        setBacklight(appConfig.display.brightness);  // Uses IO Expander
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Theme selector
    lv_obj_t *lbl_theme = lv_label_create(parent);
    lv_label_set_text(lbl_theme, "Tema de fondo");
    lv_obj_add_style(lbl_theme, &style_label, 0);
    lv_obj_set_pos(lbl_theme, 350, 140);

    lv_obj_t *theme_dd = lv_dropdown_create(parent);
    lv_dropdown_set_options(theme_dd,
        "Automatico\n"
        "Amanecer\n"
        "Atardecer\n"
        "Noche\n"
        "Cielo\n"
        "Bosque\n"
        "Oceano\n"
        "Tormenta\n"
        "Aurora");
    lv_obj_set_pos(theme_dd, 350, 165);
    lv_obj_set_size(theme_dd, 200, 40);
    lv_obj_add_event_cb(theme_dd, [](lv_event_t *e) {
        lv_obj_t *dd = lv_event_get_target(e);
        int sel = lv_dropdown_get_selected(dd);
#if !defined(USE_UI_V2) && !defined(USE_UI_V3)
        if (sel == 0) {
            setAutoBackground(getMainScreen());
        } else {
            setThemeBackground(getMainScreen(), sel - 1);
        }
#endif
        appConfig.display.background_id = sel;
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Update interval
    lv_obj_t *lbl_interval = lv_label_create(parent);
    lv_label_set_text(lbl_interval, "Actualizar cada");
    lv_obj_add_style(lbl_interval, &style_label, 0);
    lv_obj_set_pos(lbl_interval, 350, 220);

    lv_obj_t *interval_dd = lv_dropdown_create(parent);
    lv_dropdown_set_options(interval_dd, "5 min\n10 min\n15 min\n30 min\n60 min");
    lv_obj_set_pos(interval_dd, 350, 245);
    lv_obj_set_size(interval_dd, 120, 40);
    // Set current
    int intervals[] = {5, 10, 15, 30, 60};
    for (int i = 0; i < 5; i++) {
        if (appConfig.display.update_interval == intervals[i]) {
            lv_dropdown_set_selected(interval_dd, i);
            break;
        }
    }
    lv_obj_add_event_cb(interval_dd, [](lv_event_t *e) {
        lv_obj_t *dd = lv_event_get_target(e);
        int intervals[] = {5, 10, 15, 30, 60};
        appConfig.display.update_interval = intervals[lv_dropdown_get_selected(dd)];
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // 24h format toggle
    lv_obj_t *lbl_24h = lv_label_create(parent);
    lv_label_set_text(lbl_24h, "Formato 24h");
    lv_obj_add_style(lbl_24h, &style_label, 0);
    lv_obj_set_pos(lbl_24h, 500, 220);

    lv_obj_t *sw_24h = lv_switch_create(parent);
    lv_obj_set_pos(sw_24h, 500, 245);
    if (appConfig.display.use_24h) lv_obj_add_state(sw_24h, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw_24h, [](lv_event_t *e) {
        lv_obj_t *sw = lv_event_get_target(e);
        appConfig.display.use_24h = lv_obj_has_state(sw, LV_STATE_CHECKED);
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Save button
    lv_obj_t *btn_save = lv_btn_create(parent);
    lv_obj_set_pos(btn_save, 350, 310);
    lv_obj_set_size(btn_save, 150, 45);
    lv_obj_set_style_bg_color(btn_save, lv_color_hex(0x4ECDC4), 0);
    lv_obj_add_event_cb(btn_save, [](lv_event_t *e) {
        saveConfig();
        Serial.println("Configuration saved");
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *save_label = lv_label_create(btn_save);
    lv_label_set_text(save_label, LV_SYMBOL_SAVE " Guardar");
    lv_obj_set_style_text_color(save_label, lv_color_white(), 0);
    lv_obj_center(save_label);

    // Version info
    lv_obj_t *version_lbl = lv_label_create(parent);
    lv_label_set_text(version_lbl, "Estacion Clima v1.0");
    lv_obj_set_style_text_color(version_lbl, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_font(version_lbl, &lv_font_montserrat_12, 0);
    lv_obj_align(version_lbl, LV_ALIGN_BOTTOM_RIGHT, -20, -10);
}

// Include main screen modules (needed for createUI)
// Only include the active UI version to avoid variable redefinition
#if defined(USE_UI_V3)
#include "ui_main_screen_v3.h"
#elif defined(USE_UI_V2)
#include "ui_main_screen_v2.h"
#else
#include "ui_main_screen.h"
#endif

// ============================================================================
// Create Main UI - Hidden/Visible Pattern
// All screens created once, switched via LV_OBJ_FLAG_HIDDEN
// ============================================================================

void createUI() {
    hideSplashScreen();
    initStyles();

    // ========================================================================
    // Create single base screen (everything lives here)
    // ========================================================================
    base_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(base_screen, lv_color_hex(0xE8E8E8), 0);
    lv_obj_clear_flag(base_screen, LV_OBJ_FLAG_SCROLLABLE);

    // ========================================================================
    // Create MAIN container (dashboard view)
    // ========================================================================
    container_main = lv_obj_create(base_screen);
    lv_obj_set_size(container_main, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_pos(container_main, 0, 0);
    lv_obj_set_style_bg_opa(container_main, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(container_main, 0, 0);
    lv_obj_set_style_pad_all(container_main, 0, 0);
    lv_obj_clear_flag(container_main, LV_OBJ_FLAG_SCROLLABLE);

#ifdef USE_UI_V3
    createMainScreenV3Content(container_main);
#elif defined(USE_UI_V2)
    createMainScreenV2Content(container_main);
#else
    createMainScreenContent(container_main);

    // Add menu button to main container
    lv_obj_t *btn_menu = lv_btn_create(container_main);
    lv_obj_set_size(btn_menu, 60, 60);
    lv_obj_set_pos(btn_menu, SCREEN_WIDTH - 80, 20);
    lv_obj_set_style_bg_color(btn_menu, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(btn_menu, LV_OPA_30, 0);
    lv_obj_set_style_radius(btn_menu, 15, 0);
    lv_obj_set_style_border_width(btn_menu, 0, 0);
    lv_obj_add_event_cb(btn_menu, [](lv_event_t *e) {
        switchToTabsScreen();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *menu_icon = lv_label_create(btn_menu);
    lv_label_set_text(menu_icon, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_font(menu_icon, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(menu_icon, lv_color_white(), 0);
    lv_obj_center(menu_icon);
#endif

    // ========================================================================
    // Create TABS container (secondary view) - starts HIDDEN
    // ========================================================================
    container_tabs = lv_obj_create(base_screen);
    lv_obj_set_size(container_tabs, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_pos(container_tabs, 0, 0);
    lv_obj_set_style_bg_color(container_tabs, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_bg_grad_color(container_tabs, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_bg_grad_dir(container_tabs, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_border_width(container_tabs, 0, 0);
    lv_obj_set_style_pad_all(container_tabs, 0, 0);
    lv_obj_clear_flag(container_tabs, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(container_tabs, LV_OBJ_FLAG_HIDDEN);  // Start hidden

    // Legacy pointer for compatibility
    screen_main = container_tabs;

    // Create tabview inside tabs container
    tabview = lv_tabview_create(container_tabs, LV_DIR_BOTTOM, 70);
    lv_obj_set_size(tabview, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_opa(tabview, LV_OPA_TRANSP, 0);

    // Create tabs
    tab_dashboard = lv_tabview_add_tab(tabview, LV_SYMBOL_HOME " Detalle");
    tab_trends = lv_tabview_add_tab(tabview, LV_SYMBOL_REFRESH " Tendencias");
    tab_settings = lv_tabview_add_tab(tabview, LV_SYMBOL_SETTINGS " Ajustes");

    // Style tab buttons
    lv_obj_t *tab_btns = lv_tabview_get_tab_btns(tabview);
    lv_obj_set_style_bg_color(tab_btns, lv_color_hex(0x2a2a4e), 0);
    lv_obj_set_style_text_color(tab_btns, lv_color_white(), 0);
    lv_obj_set_style_border_width(tab_btns, 0, 0);

    // Add home button to tabs container
    lv_obj_t *btn_home = lv_btn_create(container_tabs);
    lv_obj_set_size(btn_home, 50, 50);
    lv_obj_set_pos(btn_home, 15, 10);
    lv_obj_set_style_bg_color(btn_home, lv_color_hex(0x4ECDC4), 0);
    lv_obj_set_style_radius(btn_home, 25, 0);
    lv_obj_add_event_cb(btn_home, [](lv_event_t *e) {
        switchToMainScreen();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *home_icon = lv_label_create(btn_home);
    lv_label_set_text(home_icon, LV_SYMBOL_HOME);
    lv_obj_set_style_text_color(home_icon, lv_color_white(), 0);
    lv_obj_center(home_icon);

    // Populate tabs
    createDashboard(tab_dashboard);
    createTrends(tab_trends);
    createSettings(tab_settings);

    // ========================================================================
    // Load base screen (main container visible, tabs hidden)
    // ========================================================================
    lv_scr_load(base_screen);

#ifdef USE_UI_V3
    createFixedLayers();  // Header/footer on layer_top
    showFixedLayers();
#endif

    currentScreenMode = SCREEN_MODE_MAIN;
    Serial.println("UI created - Hidden/Visible pattern active");
}

// ============================================================================
// Update Functions
// ============================================================================

void updateTimeDisplay() {
    // Check for stale data periodically
    dataWatchdog.checkStaleData();

    // Update main screen clock
#ifdef USE_UI_V3
    updateMainScreenV3Time();
    updateConnectionStatusV3();
#elif defined(USE_UI_V2)
    updateMainScreenV2Time();
    updateConnectionStatusV2();
#else
    updateMainScreenTime();
#endif

    // Update tabs screen clock
    if (!lbl_time || !lbl_date) return;

    char time_str[10];
    char date_str[50];

    formatTime24h(time_str, sizeof(time_str));
    formatDateLong(date_str, sizeof(date_str));

    lv_label_set_text(lbl_time, time_str);
    lv_label_set_text(lbl_date, date_str);
}

// Day name helper
const char* getDayName(int dayOffset) {
    struct tm timeinfo;
    time_t now = time(nullptr);
    now += dayOffset * 86400;  // Add days in seconds
    localtime_r(&now, &timeinfo);

    static const char* days[] = {"Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"};
    return days[timeinfo.tm_wday];
}

void updateTrendsUI() {
    extern HourlyForecast hourlyForecast[];
    extern int hourlyForecastCount;
    extern ForecastData forecast[];

    // Get current hour to start from
    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);
    int currentHour = timeinfo.tm_hour;

    // Update hourly forecast (starting from current hour)
    for (int i = 0; i < 12 && i < hourlyForecastCount; i++) {
        if (!hourly_labels[i]) continue;

        // Get hour index (wrap around if needed)
        int hourIndex = (currentHour + i) % 24;
        if (hourIndex >= hourlyForecastCount) continue;

        // Use actual hourly data
        HourlyForecast &hf = hourlyForecast[hourIndex];

        // Time label
        char time_str[10];
        snprintf(time_str, sizeof(time_str), "%02d:00", hourIndex);
        lv_label_set_text(hourly_labels[i], time_str);

        // Temperature
        char temp_str[10];
        snprintf(temp_str, sizeof(temp_str), "%.0f°", hf.temp_c);
        lv_label_set_text(hourly_temps[i], temp_str);

        // Weather icon color based on condition
        if (hf.condition_code == 1000) {
            lv_obj_set_style_text_color(hourly_icons[i], lv_color_hex(0xffd700), 0);  // Yellow for sunny
        } else if (hf.chance_of_rain > 50) {
            lv_obj_set_style_text_color(hourly_icons[i], lv_color_hex(0x64b5f6), 0);  // Blue for rainy
        } else {
            lv_obj_set_style_text_color(hourly_icons[i], lv_color_hex(0xcccccc), 0);  // Gray for cloudy
        }

        // Rain chance (only show if > 0)
        if (hf.chance_of_rain > 0) {
            char rain_str[10];
            snprintf(rain_str, sizeof(rain_str), "%d%%", hf.chance_of_rain);
            lv_label_set_text(hourly_rain[i], rain_str);
        } else {
            lv_label_set_text(hourly_rain[i], "");
        }
    }

    // Update daily forecast
    for (int i = 0; i < MAX_FORECAST_DAYS; i++) {
        if (!daily_labels[i]) continue;

        // Day name
        const char* dayName = (i == 0) ? "Hoy" : getDayName(i);
        lv_label_set_text(daily_labels[i], dayName);

        // Max temp (red)
        char max_str[10];
        snprintf(max_str, sizeof(max_str), "%.0f°", forecast[i].maxtemp_c);
        lv_label_set_text(daily_max[i], max_str);

        // Min temp (blue)
        char min_str[10];
        snprintf(min_str, sizeof(min_str), "%.0f°", forecast[i].mintemp_c);
        lv_label_set_text(daily_min[i], min_str);

        // Weather icon color
        if (forecast[i].condition_code == 1000) {
            lv_obj_set_style_text_color(daily_icons[i], lv_color_hex(0xffd700), 0);
        } else if (forecast[i].daily_chance_of_rain > 50) {
            lv_obj_set_style_text_color(daily_icons[i], lv_color_hex(0x64b5f6), 0);
        } else {
            lv_obj_set_style_text_color(daily_icons[i], lv_color_hex(0xcccccc), 0);
        }

        // Rain chance
        if (forecast[i].daily_chance_of_rain > 0) {
            char rain_str[15];
            snprintf(rain_str, sizeof(rain_str), "%d%%", forecast[i].daily_chance_of_rain);
            lv_label_set_text(daily_rain_chance[i], rain_str);
        } else {
            lv_label_set_text(daily_rain_chance[i], "");
        }
    }
}

void updateWeatherUI() {
    extern WeatherData currentWeather;
    extern ForecastData forecast[];

    // Update main screen (V1 only - V2/V3 handled by updateAllUI)
#if !defined(USE_UI_V2) && !defined(USE_UI_V3)
    updateMainScreenWeather(&currentWeather);
    updateMainScreenForecast(forecast);
#endif

    // Update trends tab
    updateTrendsUI();

    // Update tabs screen (if elements exist)
    if (!lbl_temp_outdoor) return;

    char temp_str[10];
    snprintf(temp_str, sizeof(temp_str), "%.1f", currentWeather.temp_c);
    lv_label_set_text(lbl_temp_outdoor, temp_str);

    lv_label_set_text(lbl_condition, currentWeather.condition_text);

    char hum_str[20];
    snprintf(hum_str, sizeof(hum_str), "Humedad: %d%%", currentWeather.humidity);
    lv_label_set_text(lbl_humidity, hum_str);

    char wind_str[30];
    snprintf(wind_str, sizeof(wind_str), "Viento: %.0f km/h %s",
        currentWeather.wind_kph, currentWeather.wind_dir);
    lv_label_set_text(lbl_wind, wind_str);

    char press_str[25];
    snprintf(press_str, sizeof(press_str), "Presion: %.0f hPa",
        currentWeather.pressure_mb);
    lv_label_set_text(lbl_pressure, press_str);
}

#ifdef USE_BME280
void updateIndoorUI(float temp, float humidity) {
    // Update indoor panel with BME280 readings
}
#endif

// Include history module
#include "ui_history.h"

// ============================================================================
// Switch Between Screens - Hidden/Visible Pattern
// ============================================================================

void switchToMainScreen() {
    if (currentScreenMode == SCREEN_MODE_MAIN) return;  // Already there

    currentScreenMode = SCREEN_MODE_MAIN;

    // Hide tabs, show main
    if (container_tabs) {
        lv_obj_add_flag(container_tabs, LV_OBJ_FLAG_HIDDEN);
    }
    if (container_main) {
        lv_obj_clear_flag(container_main, LV_OBJ_FLAG_HIDDEN);
    }

#ifdef USE_UI_V3
    showFixedLayers();
#endif

    Serial.println("[UI] Switched to MAIN");
}

void switchToTabsScreen() {
    if (currentScreenMode == SCREEN_MODE_TABS) return;  // Already there

    currentScreenMode = SCREEN_MODE_TABS;

#ifdef USE_UI_V3
    hideFixedLayers();
#endif

    // Hide main, show tabs
    if (container_main) {
        lv_obj_add_flag(container_main, LV_OBJ_FLAG_HIDDEN);
    }
    if (container_tabs) {
        lv_obj_clear_flag(container_tabs, LV_OBJ_FLAG_HIDDEN);
    }

    Serial.println("[UI] Switched to TABS");
}

// ============================================================================
// Unified Update Function
// ============================================================================

void updateAllUI() {
    extern WeatherData currentWeather;
    extern ForecastData forecast[];

    // Update main screen
#ifdef USE_UI_V3
    updateMainScreenV3Weather(&currentWeather);
    updateMainScreenV3Forecast(forecast);
    updateConnectionStatusV3();
#elif defined(USE_UI_V2)
    updateMainScreenV2Weather(&currentWeather);
    updateMainScreenV2Forecast(forecast);
    updateConnectionStatusV2();
#else
    updateMainScreenWeather(&currentWeather);
    updateMainScreenForecast(forecast);
#endif

    // Update tab screens
    updateWeatherUI();
    updateHistoryUI();
}

#endif // UI_H
