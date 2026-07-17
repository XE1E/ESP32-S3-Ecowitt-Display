/**
 * ui_detail_principal.h - Pantalla detalle de estación principal
 *
 * Fase 1: Muestra todos los datos de la estación Ecowitt principal
 * - Temperatura con sensación térmica y punto de rocío
 * - Min/Max con hora de los extremos
 * - Pronóstico barométrico (texto)
 * - Comparación vs ayer
 * - Lluvia acumulada (día, semana, mes, año)
 * - UV y radiación solar
 * - Calidad del aire (AQI)
 * - Sol y luna
 */

#ifndef UI_DETAIL_PRINCIPAL_H
#define UI_DETAIL_PRINCIPAL_H

#include <lvgl.h>
#include "config.h"
#include "weather_icons.h"

// Referencias a labels para actualización
static struct {
    lv_obj_t *screen;
    // Temperatura
    lv_obj_t *temp_value;
    lv_obj_t *feels_like;
    lv_obj_t *dew_point;
    lv_obj_t *temp_max;
    lv_obj_t *temp_min;
    lv_obj_t *temp_compare;
    // Humedad y presión
    lv_obj_t *humidity;
    lv_obj_t *pressure;
    lv_obj_t *pressure_trend;
    lv_obj_t *forecast_text;
    // Viento
    lv_obj_t *wind_speed;
    lv_obj_t *wind_dir;
    lv_obj_t *wind_gust;
    lv_obj_t *wind_max;
    lv_obj_t *wind_beaufort;
    // Lluvia
    lv_obj_t *rain_rate;
    lv_obj_t *rain_day;
    lv_obj_t *rain_week;
    lv_obj_t *rain_month;
    lv_obj_t *rain_year;
    // UV y solar
    lv_obj_t *uv_index;
    lv_obj_t *uv_level;
    lv_obj_t *solar_radiation;
    // AQI
    lv_obj_t *aqi_value;
    lv_obj_t *aqi_level;
    lv_obj_t *aqi_indicator;
    // Sol y luna
    lv_obj_t *sunrise;
    lv_obj_t *sunset;
    lv_obj_t *moon_phase;
    lv_obj_t *moon_illum;
} detail_principal_ui;

// Colores
#define DP_COLOR_TEMP       lv_color_hex(0xFF6B6B)  // Rojo suave
#define DP_COLOR_HUMIDITY   lv_color_hex(0x67E8F9)  // Cyan
#define DP_COLOR_PRESSURE   lv_color_hex(0xC4B5FD)  // Violeta
#define DP_COLOR_WIND       lv_color_hex(0x6EE7B7)  // Verde
#define DP_COLOR_RAIN       lv_color_hex(0x93C5FD)  // Azul
#define DP_COLOR_UV         lv_color_hex(0xFCD34D)  // Amarillo
#define DP_COLOR_SUN        lv_color_hex(0xFFB800)  // Naranja

// Forward declarations
extern bool darkMode;
extern lv_style_t style_card;
void navigateToScreenById(int screenId);
void enableSwipeNavigation(lv_obj_t *screen);

// Colores según tema
static lv_color_t dp_text_primary() { return darkMode ? lv_color_hex(0xE8EDF5) : lv_color_hex(0x16232F); }
static lv_color_t dp_text_muted() { return darkMode ? lv_color_hex(0x8496A6) : lv_color_hex(0x55677A); }
static lv_color_t dp_bg() { return darkMode ? lv_color_hex(0x0A1422) : lv_color_hex(0xF8FAFC); }
static lv_color_t dp_card_bg() { return darkMode ? lv_color_hex(0x1E2D40) : lv_color_hex(0xFFFFFF); }
static lv_color_t dp_border() { return darkMode ? lv_color_hex(0x3A4A5A) : lv_color_hex(0xE2E8F0); }

/**
 * Obtener color del AQI según valor
 */
static lv_color_t getAqiColor(int aqi) {
    if (aqi <= 50) return lv_color_hex(0x4ADE80);       // Bueno - Verde
    if (aqi <= 100) return lv_color_hex(0xFCD34D);      // Moderado - Amarillo
    if (aqi <= 150) return lv_color_hex(0xFB923C);      // Danino sensibles - Naranja
    if (aqi <= 200) return lv_color_hex(0xF87171);      // Danino - Rojo
    if (aqi <= 300) return lv_color_hex(0xC084FC);      // Muy danino - Purpura
    return lv_color_hex(0x881337);                       // Peligroso - Marrón
}

/**
 * Obtener texto del nivel AQI
 */
static const char* getAqiLevel(int aqi) {
    if (aqi <= 50) return "Bueno";
    if (aqi <= 100) return "Moderado";
    if (aqi <= 150) return "Danino (sensibles)";
    if (aqi <= 200) return "Danino";
    if (aqi <= 300) return "Muy danino";
    return "Peligroso";
}

/**
 * Obtener texto del nivel UV
 */
static const char* getUvLevel(float uv) {
    if (uv < 3) return "Bajo";
    if (uv < 6) return "Moderado";
    if (uv < 8) return "Alto";
    if (uv < 11) return "Muy alto";
    return "Extremo";
}

/**
 * Obtener color del UV
 */
static lv_color_t getUvColor(float uv) {
    if (uv < 3) return lv_color_hex(0x4ADE80);   // Verde
    if (uv < 6) return lv_color_hex(0xFCD34D);   // Amarillo
    if (uv < 8) return lv_color_hex(0xFB923C);   // Naranja
    if (uv < 11) return lv_color_hex(0xF87171);  // Rojo
    return lv_color_hex(0xC084FC);                // Púrpura
}

/**
 * Escala Beaufort
 */
static const char* getBeaufortDesc(float kmh) {
    if (kmh < 1) return "Calma";
    if (kmh < 6) return "Ventolina";
    if (kmh < 12) return "Brisa muy debil";
    if (kmh < 20) return "Brisa debil";
    if (kmh < 29) return "Brisa moderada";
    if (kmh < 39) return "Brisa fresca";
    if (kmh < 50) return "Brisa fuerte";
    if (kmh < 62) return "Viento fuerte";
    if (kmh < 75) return "Temporal";
    if (kmh < 89) return "Temporal fuerte";
    if (kmh < 103) return "Temporal muy fuerte";
    if (kmh < 118) return "Tempestad";
    return "Huracan";
}

/**
 * Crear card con barra de acento y sombra
 */
static lv_obj_t* createCard(lv_obj_t *parent, int x, int y, int w, int h, const char* title, lv_color_t accent) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    // Sombra explicita (igual que dashboard)
    lv_obj_set_style_shadow_width(card, 25, 0);
    lv_obj_set_style_shadow_color(card, darkMode ? lv_color_hex(0x000000) : lv_color_hex(0x374151), 0);
    lv_obj_set_style_shadow_opa(card, darkMode ? LV_OPA_60 : LV_OPA_50, 0);
    lv_obj_set_style_shadow_ofs_x(card, 2, 0);
    lv_obj_set_style_shadow_ofs_y(card, 6, 0);
    lv_obj_set_style_shadow_spread(card, 4, 0);

    // Barra de acento superior
    lv_obj_t *accent_bar = lv_obj_create(card);
    lv_obj_set_size(accent_bar, w - 24, 3);
    lv_obj_align(accent_bar, LV_ALIGN_TOP_MID, 0, -8);
    lv_obj_set_style_bg_color(accent_bar, accent, 0);
    lv_obj_set_style_radius(accent_bar, 2, 0);
    lv_obj_set_style_border_width(accent_bar, 0, 0);

    // Titulo
    lv_obj_t *lbl = lv_label_create(card);
    lv_label_set_text(lbl, title);
    lv_obj_set_style_text_color(lbl, dp_text_muted(), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 8, 5);

    return card;
}

/**
 * Crear pantalla detalle principal
 */
void createDetailPrincipal() {
    extern WeatherData g_weather;
    extern CompareData g_compare;
    extern AlmanacData g_almanac;

    lv_obj_t *scr = lv_obj_create(NULL);
    detail_principal_ui.screen = scr;
    lv_obj_set_style_bg_color(scr, dp_bg(), 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    // === HEADER ===
    lv_obj_t *header = lv_obj_create(scr);
    lv_obj_set_size(header, SCREEN_WIDTH, 60);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, dp_card_bg(), 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(header, dp_border(), 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    // Botón volver
    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_set_size(btn_back, 90, 34);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 12, 13);
    lv_obj_set_style_bg_color(btn_back, darkMode ? lv_color_hex(0x2A3A4A) : lv_color_hex(0xE8EDF5), 0);
    lv_obj_set_style_radius(btn_back, 6, 0);
    lv_obj_set_style_shadow_width(btn_back, 0, 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        navigateToScreenById(0);  // SCREEN_DASHBOARD
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_lbl = lv_label_create(btn_back);
    lv_label_set_text(btn_lbl, LV_SYMBOL_LEFT " Volver");
    lv_obj_set_style_text_color(btn_lbl, dp_text_primary(), 0);
    lv_obj_center(btn_lbl);

    // Título
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "ESTACION PRINCIPAL");
    lv_obj_set_style_text_color(title, dp_text_primary(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 18);

    // Subtítulo
    lv_obj_t *subtitle = lv_label_create(scr);
    lv_label_set_text(subtitle, "Ecowitt WS2910 + WS69");
    lv_obj_set_style_text_color(subtitle, DP_COLOR_TEMP, 0);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 42);

    int y = 68;
    int gap = 10;
    int row1_h = 250;  // Altura fila 1 (era 175)
    int row2_h = 240;  // Altura fila 2 (era 170)
    int col1_w = 320;
    int col2_w = 340;
    int col3_w = 330;

    // === FILA 1: Temperatura | Humedad/Presion | Viento ===

    // Card Temperatura
    lv_obj_t *card_temp = createCard(scr, gap, y, col1_w, row1_h, "TEMPERATURA", DP_COLOR_TEMP);

    lv_obj_t *temp_icon = lv_label_create(card_temp);
    lv_label_set_text(temp_icon, WI_THERMOMETER);
    lv_obj_set_style_text_font(temp_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(temp_icon, DP_COLOR_TEMP, 0);
    lv_obj_align(temp_icon, LV_ALIGN_TOP_LEFT, 5, 25);

    detail_principal_ui.temp_value = lv_label_create(card_temp);
    lv_label_set_text(detail_principal_ui.temp_value, "--.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.temp_value, dp_text_primary(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.temp_value, &lv_font_montserrat_40, 0);
    lv_obj_align(detail_principal_ui.temp_value, LV_ALIGN_TOP_LEFT, 65, 25);

    detail_principal_ui.feels_like = lv_label_create(card_temp);
    lv_label_set_text(detail_principal_ui.feels_like, "ST: --.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.feels_like, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.feels_like, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.feels_like, LV_ALIGN_TOP_LEFT, 10, 78);

    detail_principal_ui.dew_point = lv_label_create(card_temp);
    lv_label_set_text(detail_principal_ui.dew_point, "Rocio: --.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.dew_point, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.dew_point, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.dew_point, LV_ALIGN_TOP_LEFT, 10, 100);

    detail_principal_ui.temp_max = lv_label_create(card_temp);
    lv_label_set_text(detail_principal_ui.temp_max, LV_SYMBOL_UP " --.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.temp_max, DP_COLOR_TEMP, 0);
    lv_obj_set_style_text_font(detail_principal_ui.temp_max, &lv_font_montserrat_16, 0);
    lv_obj_align(detail_principal_ui.temp_max, LV_ALIGN_TOP_LEFT, 10, 150);

    detail_principal_ui.temp_min = lv_label_create(card_temp);
    lv_label_set_text(detail_principal_ui.temp_min, LV_SYMBOL_DOWN " --.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.temp_min, lv_color_hex(0x64B5F6), 0);
    lv_obj_set_style_text_font(detail_principal_ui.temp_min, &lv_font_montserrat_16, 0);
    lv_obj_align(detail_principal_ui.temp_min, LV_ALIGN_TOP_LEFT, 10, 175);

    detail_principal_ui.temp_compare = lv_label_create(card_temp);
    lv_label_set_text(detail_principal_ui.temp_compare, "vs ayer: --");
    lv_obj_set_style_text_color(detail_principal_ui.temp_compare, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.temp_compare, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.temp_compare, LV_ALIGN_TOP_LEFT, 10, 205);

    // Card Humedad/Presion
    lv_obj_t *card_hp = createCard(scr, gap + col1_w + gap, y, col2_w, row1_h, "HUMEDAD / PRESION", DP_COLOR_HUMIDITY);

    lv_obj_t *hum_icon = lv_label_create(card_hp);
    lv_label_set_text(hum_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(hum_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(hum_icon, DP_COLOR_HUMIDITY, 0);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_LEFT, 5, 28);

    detail_principal_ui.humidity = lv_label_create(card_hp);
    lv_label_set_text(detail_principal_ui.humidity, "--%");
    lv_obj_set_style_text_color(detail_principal_ui.humidity, DP_COLOR_HUMIDITY, 0);
    lv_obj_set_style_text_font(detail_principal_ui.humidity, &lv_font_montserrat_28, 0);
    lv_obj_align(detail_principal_ui.humidity, LV_ALIGN_TOP_LEFT, 50, 25);

    lv_obj_t *pres_icon = lv_label_create(card_hp);
    lv_label_set_text(pres_icon, WI_BAROMETER);
    lv_obj_set_style_text_font(pres_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(pres_icon, DP_COLOR_PRESSURE, 0);
    lv_obj_align(pres_icon, LV_ALIGN_TOP_LEFT, 5, 85);

    detail_principal_ui.pressure = lv_label_create(card_hp);
    lv_label_set_text(detail_principal_ui.pressure, "---- hPa");
    lv_obj_set_style_text_color(detail_principal_ui.pressure, DP_COLOR_PRESSURE, 0);
    lv_obj_set_style_text_font(detail_principal_ui.pressure, &lv_font_montserrat_28, 0);
    lv_obj_align(detail_principal_ui.pressure, LV_ALIGN_TOP_LEFT, 50, 83);

    detail_principal_ui.pressure_trend = lv_label_create(card_hp);
    lv_label_set_text(detail_principal_ui.pressure_trend, "Tendencia: --");
    lv_obj_set_style_text_color(detail_principal_ui.pressure_trend, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.pressure_trend, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.pressure_trend, LV_ALIGN_TOP_LEFT, 10, 125);

    detail_principal_ui.forecast_text = lv_label_create(card_hp);
    lv_label_set_text(detail_principal_ui.forecast_text, "Pronostico: --");
    lv_obj_set_style_text_color(detail_principal_ui.forecast_text, lv_color_hex(0x4ADE80), 0);
    lv_obj_set_style_text_font(detail_principal_ui.forecast_text, &lv_font_montserrat_14, 0);
    lv_obj_set_width(detail_principal_ui.forecast_text, col2_w - 20);
    lv_label_set_long_mode(detail_principal_ui.forecast_text, LV_LABEL_LONG_WRAP);
    lv_obj_align(detail_principal_ui.forecast_text, LV_ALIGN_TOP_LEFT, 10, 155);

    // Card Viento
    lv_obj_t *card_wind = createCard(scr, gap + col1_w + gap + col2_w + gap, y, col3_w, row1_h, "VIENTO", DP_COLOR_WIND);

    lv_obj_t *wind_icon = lv_label_create(card_wind);
    lv_label_set_text(wind_icon, WI_STRONG_WIND);
    lv_obj_set_style_text_font(wind_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(wind_icon, DP_COLOR_WIND, 0);
    lv_obj_align(wind_icon, LV_ALIGN_TOP_LEFT, 5, 25);

    detail_principal_ui.wind_speed = lv_label_create(card_wind);
    lv_label_set_text(detail_principal_ui.wind_speed, "-- km/h");
    lv_obj_set_style_text_color(detail_principal_ui.wind_speed, DP_COLOR_WIND, 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_speed, &lv_font_montserrat_28, 0);
    lv_obj_align(detail_principal_ui.wind_speed, LV_ALIGN_TOP_LEFT, 60, 25);

    detail_principal_ui.wind_dir = lv_label_create(card_wind);
    lv_label_set_text(detail_principal_ui.wind_dir, "Dir: -- ---°");
    lv_obj_set_style_text_color(detail_principal_ui.wind_dir, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_dir, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.wind_dir, LV_ALIGN_TOP_LEFT, 10, 65);

    detail_principal_ui.wind_gust = lv_label_create(card_wind);
    lv_label_set_text(detail_principal_ui.wind_gust, "Rafagas: -- km/h");
    lv_obj_set_style_text_color(detail_principal_ui.wind_gust, lv_color_hex(0xFDE047), 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_gust, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.wind_gust, LV_ALIGN_TOP_LEFT, 10, 90);

    detail_principal_ui.wind_max = lv_label_create(card_wind);
    lv_label_set_text(detail_principal_ui.wind_max, "Max hoy: -- km/h");
    lv_obj_set_style_text_color(detail_principal_ui.wind_max, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_max, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.wind_max, LV_ALIGN_TOP_LEFT, 10, 115);

    detail_principal_ui.wind_beaufort = lv_label_create(card_wind);
    lv_label_set_text(detail_principal_ui.wind_beaufort, "Beaufort: --");
    lv_obj_set_style_text_color(detail_principal_ui.wind_beaufort, DP_COLOR_WIND, 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_beaufort, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.wind_beaufort, LV_ALIGN_TOP_LEFT, 10, 190);

    // === FILA 2: Lluvia | UV/Solar | AQI | Sol/Luna ===
    int y2 = y + row1_h + gap;
    int row2_w = (SCREEN_WIDTH - gap * 5) / 4;

    // Card Lluvia
    lv_obj_t *card_rain = createCard(scr, gap, y2, row2_w, row2_h, "PRECIPITACION", DP_COLOR_RAIN);

    lv_obj_t *rain_icon = lv_label_create(card_rain);
    lv_label_set_text(rain_icon, WI_RAIN);
    lv_obj_set_style_text_font(rain_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(rain_icon, DP_COLOR_RAIN, 0);
    lv_obj_align(rain_icon, LV_ALIGN_TOP_LEFT, 5, 25);

    detail_principal_ui.rain_rate = lv_label_create(card_rain);
    lv_label_set_text(detail_principal_ui.rain_rate, "-.-- mm/h");
    lv_obj_set_style_text_color(detail_principal_ui.rain_rate, DP_COLOR_RAIN, 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_rate, &lv_font_montserrat_24, 0);
    lv_obj_align(detail_principal_ui.rain_rate, LV_ALIGN_TOP_LEFT, 50, 25);

    detail_principal_ui.rain_day = lv_label_create(card_rain);
    lv_label_set_text(detail_principal_ui.rain_day, "Hoy: -- mm");
    lv_obj_set_style_text_color(detail_principal_ui.rain_day, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_day, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.rain_day, LV_ALIGN_TOP_LEFT, 10, 60);

    detail_principal_ui.rain_week = lv_label_create(card_rain);
    lv_label_set_text(detail_principal_ui.rain_week, "Semana: -- mm");
    lv_obj_set_style_text_color(detail_principal_ui.rain_week, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_week, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.rain_week, LV_ALIGN_TOP_LEFT, 10, 85);

    detail_principal_ui.rain_month = lv_label_create(card_rain);
    lv_label_set_text(detail_principal_ui.rain_month, "Mes: -- mm");
    lv_obj_set_style_text_color(detail_principal_ui.rain_month, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_month, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.rain_month, LV_ALIGN_TOP_LEFT, 10, 110);

    detail_principal_ui.rain_year = lv_label_create(card_rain);
    lv_label_set_text(detail_principal_ui.rain_year, "Anual: -- mm");
    lv_obj_set_style_text_color(detail_principal_ui.rain_year, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_year, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.rain_year, LV_ALIGN_TOP_LEFT, 10, 135);

    // Card UV/Solar
    lv_obj_t *card_uv = createCard(scr, gap + row2_w + gap, y2, row2_w, row2_h, "UV / SOLAR", DP_COLOR_UV);

    lv_obj_t *uv_icon = lv_label_create(card_uv);
    lv_label_set_text(uv_icon, WI_DAY_SUNNY);
    lv_obj_set_style_text_font(uv_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(uv_icon, DP_COLOR_UV, 0);
    lv_obj_align(uv_icon, LV_ALIGN_TOP_LEFT, 5, 25);

    detail_principal_ui.uv_index = lv_label_create(card_uv);
    lv_label_set_text(detail_principal_ui.uv_index, "UV: --");
    lv_obj_set_style_text_color(detail_principal_ui.uv_index, DP_COLOR_UV, 0);
    lv_obj_set_style_text_font(detail_principal_ui.uv_index, &lv_font_montserrat_24, 0);
    lv_obj_align(detail_principal_ui.uv_index, LV_ALIGN_TOP_LEFT, 50, 25);

    detail_principal_ui.uv_level = lv_label_create(card_uv);
    lv_label_set_text(detail_principal_ui.uv_level, "(---)");
    lv_obj_set_style_text_color(detail_principal_ui.uv_level, DP_COLOR_UV, 0);
    lv_obj_set_style_text_font(detail_principal_ui.uv_level, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.uv_level, LV_ALIGN_TOP_LEFT, 10, 70);

    detail_principal_ui.solar_radiation = lv_label_create(card_uv);
    lv_label_set_text(detail_principal_ui.solar_radiation, "Solar: -- W/m2");
    lv_obj_set_style_text_color(detail_principal_ui.solar_radiation, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.solar_radiation, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.solar_radiation, LV_ALIGN_TOP_LEFT, 10, 110);

    // Card AQI
    lv_obj_t *card_aqi = createCard(scr, gap + (row2_w + gap) * 2, y2, row2_w, row2_h, "CALIDAD AIRE", lv_color_hex(0x4ADE80));

    detail_principal_ui.aqi_indicator = lv_obj_create(card_aqi);
    lv_obj_set_size(detail_principal_ui.aqi_indicator, 60, 60);
    lv_obj_align(detail_principal_ui.aqi_indicator, LV_ALIGN_TOP_LEFT, 10, 35);
    lv_obj_set_style_bg_color(detail_principal_ui.aqi_indicator, lv_color_hex(0x4ADE80), 0);
    lv_obj_set_style_radius(detail_principal_ui.aqi_indicator, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(detail_principal_ui.aqi_indicator, 0, 0);

    detail_principal_ui.aqi_value = lv_label_create(card_aqi);
    lv_label_set_text(detail_principal_ui.aqi_value, "AQI: --");
    lv_obj_set_style_text_color(detail_principal_ui.aqi_value, dp_text_primary(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.aqi_value, &lv_font_montserrat_24, 0);
    lv_obj_align(detail_principal_ui.aqi_value, LV_ALIGN_TOP_LEFT, 80, 35);

    detail_principal_ui.aqi_level = lv_label_create(card_aqi);
    lv_label_set_text(detail_principal_ui.aqi_level, "---");
    lv_obj_set_style_text_color(detail_principal_ui.aqi_level, lv_color_hex(0x4ADE80), 0);
    lv_obj_set_style_text_font(detail_principal_ui.aqi_level, &lv_font_montserrat_16, 0);
    lv_obj_align(detail_principal_ui.aqi_level, LV_ALIGN_TOP_LEFT, 80, 70);

    lv_obj_t *aqi_note = lv_label_create(card_aqi);
    lv_label_set_text(aqi_note, "Fuente: WAQI");
    lv_obj_set_style_text_color(aqi_note, dp_text_muted(), 0);
    lv_obj_set_style_text_font(aqi_note, &lv_font_montserrat_12, 0);
    lv_obj_align(aqi_note, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    // Card Sol/Luna
    lv_obj_t *card_sun = createCard(scr, gap + (row2_w + gap) * 3, y2, row2_w, row2_h, "SOL / LUNA", DP_COLOR_SUN);

    lv_obj_t *sun_icon = lv_label_create(card_sun);
    lv_label_set_text(sun_icon, WI_SUNRISE);
    lv_obj_set_style_text_font(sun_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(sun_icon, DP_COLOR_SUN, 0);
    lv_obj_align(sun_icon, LV_ALIGN_TOP_LEFT, 10, 28);

    detail_principal_ui.sunrise = lv_label_create(card_sun);
    lv_label_set_text(detail_principal_ui.sunrise, "--:--");
    lv_obj_set_style_text_color(detail_principal_ui.sunrise, DP_COLOR_SUN, 0);
    lv_obj_set_style_text_font(detail_principal_ui.sunrise, &lv_font_montserrat_18, 0);
    lv_obj_align(detail_principal_ui.sunrise, LV_ALIGN_TOP_LEFT, 50, 30);

    lv_obj_t *set_icon = lv_label_create(card_sun);
    lv_label_set_text(set_icon, WI_SUNSET);
    lv_obj_set_style_text_font(set_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(set_icon, lv_color_hex(0xFB923C), 0);
    lv_obj_align(set_icon, LV_ALIGN_TOP_LEFT, 10, 62);

    detail_principal_ui.sunset = lv_label_create(card_sun);
    lv_label_set_text(detail_principal_ui.sunset, "--:--");
    lv_obj_set_style_text_color(detail_principal_ui.sunset, lv_color_hex(0xFB923C), 0);
    lv_obj_set_style_text_font(detail_principal_ui.sunset, &lv_font_montserrat_18, 0);
    lv_obj_align(detail_principal_ui.sunset, LV_ALIGN_TOP_LEFT, 50, 64);

    lv_obj_t *moon_icon = lv_label_create(card_sun);
    lv_label_set_text(moon_icon, WI_MOON_WAXING_CRES);
    lv_obj_set_style_text_font(moon_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(moon_icon, lv_color_hex(0xA5B4C4), 0);
    lv_obj_align(moon_icon, LV_ALIGN_TOP_LEFT, 10, 100);

    detail_principal_ui.moon_phase = lv_label_create(card_sun);
    lv_label_set_text(detail_principal_ui.moon_phase, "---");
    lv_obj_set_style_text_color(detail_principal_ui.moon_phase, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.moon_phase, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.moon_phase, LV_ALIGN_TOP_LEFT, 50, 100);

    detail_principal_ui.moon_illum = lv_label_create(card_sun);
    lv_label_set_text(detail_principal_ui.moon_illum, "Ilum: --%");
    lv_obj_set_style_text_color(detail_principal_ui.moon_illum, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.moon_illum, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.moon_illum, LV_ALIGN_TOP_LEFT, 50, 125);

    // Footer con indicador swipe
    lv_obj_t *footer = lv_label_create(scr);
    lv_label_set_text(footer, LV_SYMBOL_LEFT " Swipe para mas " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(footer, dp_text_muted(), 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -8);

    // Habilitar swipe para navegación
    enableSwipeNavigation(scr);

    Serial.println("[UI] Pantalla detalle Principal creada");
}

/**
 * Actualizar pantalla detalle principal con datos actuales
 */
void updateDetailPrincipal() {
    extern WeatherData g_weather;
    extern CompareData g_compare;
    extern AlmanacData g_almanac;
    extern ForecastData g_forecast;
    extern AirQualityData g_airquality;
    extern StatsData g_stats;

    if (!detail_principal_ui.screen) return;

    char buf[64];

    // Temperatura
    snprintf(buf, sizeof(buf), "%.1f°C", g_weather.temp_outdoor);
    lv_label_set_text(detail_principal_ui.temp_value, buf);

    snprintf(buf, sizeof(buf), "ST: %.1f°C", g_weather.feels_like);
    lv_label_set_text(detail_principal_ui.feels_like, buf);

    snprintf(buf, sizeof(buf), "Rocio: %.1f°C", g_weather.dewpoint);
    lv_label_set_text(detail_principal_ui.dew_point, buf);

    // Min/Max con hora si está disponible
    if (g_stats.valid && strlen(g_stats.temp_max_time) > 0) {
        snprintf(buf, sizeof(buf), LV_SYMBOL_UP " %.1f°C (%s)", g_stats.temp_max, g_stats.temp_max_time);
    } else {
        snprintf(buf, sizeof(buf), LV_SYMBOL_UP " %.1f°C", g_weather.temp_max);
    }
    lv_label_set_text(detail_principal_ui.temp_max, buf);

    if (g_stats.valid && strlen(g_stats.temp_min_time) > 0) {
        snprintf(buf, sizeof(buf), LV_SYMBOL_DOWN " %.1f°C (%s)", g_stats.temp_min, g_stats.temp_min_time);
    } else {
        snprintf(buf, sizeof(buf), LV_SYMBOL_DOWN " %.1f°C", g_weather.temp_min);
    }
    lv_label_set_text(detail_principal_ui.temp_min, buf);

    if (g_compare.valid) {
        snprintf(buf, sizeof(buf), "vs ayer: %+.1f°C", g_compare.temp_diff);
        lv_label_set_text(detail_principal_ui.temp_compare, buf);
        lv_obj_set_style_text_color(detail_principal_ui.temp_compare,
            g_compare.temp_diff > 0 ? DP_COLOR_TEMP : lv_color_hex(0x64B5F6), 0);
    }

    // Humedad y Presión
    snprintf(buf, sizeof(buf), "%.0f%%", g_weather.humidity_outdoor);
    lv_label_set_text(detail_principal_ui.humidity, buf);

    snprintf(buf, sizeof(buf), "%.1f hPa", g_weather.pressure_rel);
    lv_label_set_text(detail_principal_ui.pressure, buf);

    // Tendencia de presion desde compare
    if (g_compare.valid) {
        if (g_compare.pressure_diff > 0.5) {
            snprintf(buf, sizeof(buf), "Tend: " LV_SYMBOL_UP " +%.1f hPa", g_compare.pressure_diff);
            lv_obj_set_style_text_color(detail_principal_ui.pressure_trend, lv_color_hex(0x4ADE80), 0);
        } else if (g_compare.pressure_diff < -0.5) {
            snprintf(buf, sizeof(buf), "Tend: " LV_SYMBOL_DOWN " %.1f hPa", g_compare.pressure_diff);
            lv_obj_set_style_text_color(detail_principal_ui.pressure_trend, lv_color_hex(0xF87171), 0);
        } else {
            snprintf(buf, sizeof(buf), "Tend: " LV_SYMBOL_RIGHT " Estable");
            lv_obj_set_style_text_color(detail_principal_ui.pressure_trend, dp_text_muted(), 0);
        }
        lv_label_set_text(detail_principal_ui.pressure_trend, buf);
    } else {
        lv_label_set_text(detail_principal_ui.pressure_trend, "Tend: --");
    }

    // Pronóstico barométrico
    if (g_forecast.valid && g_forecast.available) {
        lv_label_set_text(detail_principal_ui.forecast_text, g_forecast.forecast);
        // Color según tendencia: rising=verde, falling=rojo, steady=azul
        if (strstr(g_forecast.trend_code, "rising") != nullptr) {
            lv_obj_set_style_text_color(detail_principal_ui.forecast_text, lv_color_hex(0x4ADE80), 0);
        } else if (strstr(g_forecast.trend_code, "falling") != nullptr) {
            lv_obj_set_style_text_color(detail_principal_ui.forecast_text, lv_color_hex(0xF87171), 0);
        } else {
            lv_obj_set_style_text_color(detail_principal_ui.forecast_text, lv_color_hex(0x60A5FA), 0);
        }
    } else {
        lv_label_set_text(detail_principal_ui.forecast_text, "Pronostico: Sin datos");
        lv_obj_set_style_text_color(detail_principal_ui.forecast_text, dp_text_muted(), 0);
    }

    // Viento
    snprintf(buf, sizeof(buf), "%.0f km/h", g_weather.wind_speed);
    lv_label_set_text(detail_principal_ui.wind_speed, buf);

    snprintf(buf, sizeof(buf), "Dir: %s %.0f°", g_weather.wind_dir_cardinal, g_weather.wind_dir);
    lv_label_set_text(detail_principal_ui.wind_dir, buf);

    snprintf(buf, sizeof(buf), "Rafagas: %.0f km/h", g_weather.wind_gust);
    lv_label_set_text(detail_principal_ui.wind_gust, buf);

    if (g_stats.valid && strlen(g_stats.wind_max_time) > 0) {
        snprintf(buf, sizeof(buf), "Max: %.0f km/h (%s)", g_stats.wind_max, g_stats.wind_max_time);
    } else {
        snprintf(buf, sizeof(buf), "Max hoy: %.0f km/h", g_weather.wind_max);
    }
    lv_label_set_text(detail_principal_ui.wind_max, buf);

    snprintf(buf, sizeof(buf), "%s", getBeaufortDesc(g_weather.wind_speed));
    lv_label_set_text(detail_principal_ui.wind_beaufort, buf);

    // Lluvia
    snprintf(buf, sizeof(buf), "%.1f mm/h", g_weather.rain_rate);
    lv_label_set_text(detail_principal_ui.rain_rate, buf);

    snprintf(buf, sizeof(buf), "Hoy: %.1f mm", g_weather.rain_day);
    lv_label_set_text(detail_principal_ui.rain_day, buf);

    snprintf(buf, sizeof(buf), "Semana: %.1f mm", g_weather.rain_week);
    lv_label_set_text(detail_principal_ui.rain_week, buf);

    snprintf(buf, sizeof(buf), "Mes: %.1f mm", g_weather.rain_month);
    lv_label_set_text(detail_principal_ui.rain_month, buf);

    snprintf(buf, sizeof(buf), "Anual: %.1f mm", g_weather.rain_year);
    lv_label_set_text(detail_principal_ui.rain_year, buf);

    // UV
    snprintf(buf, sizeof(buf), "UV: %.0f", g_weather.uv);
    lv_label_set_text(detail_principal_ui.uv_index, buf);
    lv_obj_set_style_text_color(detail_principal_ui.uv_index, getUvColor(g_weather.uv), 0);

    snprintf(buf, sizeof(buf), "(%s)", getUvLevel(g_weather.uv));
    lv_label_set_text(detail_principal_ui.uv_level, buf);
    lv_obj_set_style_text_color(detail_principal_ui.uv_level, getUvColor(g_weather.uv), 0);

    snprintf(buf, sizeof(buf), "Solar: %.0f W/m2", g_weather.solar_radiation);
    lv_label_set_text(detail_principal_ui.solar_radiation, buf);

    // AQI - Calidad del aire
    if (g_airquality.valid && g_airquality.aqi > 0) {
        snprintf(buf, sizeof(buf), "AQI: %d", g_airquality.aqi);
        lv_label_set_text(detail_principal_ui.aqi_value, buf);
        lv_label_set_text(detail_principal_ui.aqi_level, getAqiLevel(g_airquality.aqi));
        lv_color_t aqi_color = getAqiColor(g_airquality.aqi);
        lv_obj_set_style_text_color(detail_principal_ui.aqi_level, aqi_color, 0);
        lv_obj_set_style_bg_color(detail_principal_ui.aqi_indicator, aqi_color, 0);
    } else {
        lv_label_set_text(detail_principal_ui.aqi_value, "AQI: --");
        lv_label_set_text(detail_principal_ui.aqi_level, "Sin datos");
        lv_obj_set_style_text_color(detail_principal_ui.aqi_level, dp_text_muted(), 0);
        lv_obj_set_style_bg_color(detail_principal_ui.aqi_indicator, dp_text_muted(), 0);
    }

    // Sol y Luna
    if (g_almanac.valid) {
        lv_label_set_text(detail_principal_ui.sunrise, g_almanac.sunrise);
        lv_label_set_text(detail_principal_ui.sunset, g_almanac.sunset);
        lv_label_set_text(detail_principal_ui.moon_phase, g_almanac.moon_phase);

        snprintf(buf, sizeof(buf), "Ilum: %d%%", g_almanac.moon_illumination);
        lv_label_set_text(detail_principal_ui.moon_illum, buf);
    }
}

/**
 * Obtener referencia a la pantalla
 */
lv_obj_t* getDetailPrincipalScreen() {
    return detail_principal_ui.screen;
}

#endif // UI_DETAIL_PRINCIPAL_H
