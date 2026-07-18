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
    lv_obj_t *moon_icon;
    lv_obj_t *moon_phase;
    lv_obj_t *moon_illum;
    // Baterias
    lv_obj_t *battery_ws69;
    lv_obj_t *battery_wn31;
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

// Paleta de 3 tonos grises consistente (sin tintes verdes/azules)
// Tono 1: Fondo - negro/casi negro
// Tono 2: Cards principales / Header - gris oscuro
// Tono 3: Sub-cards / Botones - gris más claro
static lv_color_t dp_bg_dark()      { return lv_color_hex(0x0A0E12); }  // Tono 1: casi negro
static lv_color_t dp_card_dark()    { return lv_color_hex(0x1A1F24); }  // Tono 2: gris oscuro
static lv_color_t dp_subcard_dark() { return lv_color_hex(0x2A3238); }  // Tono 3: gris medio

static lv_color_t dp_text_primary() { return darkMode ? lv_color_hex(0xE8EDF5) : lv_color_hex(0x16232F); }
static lv_color_t dp_text_muted() { return darkMode ? lv_color_hex(0x8496A6) : lv_color_hex(0x55677A); }
static lv_color_t dp_bg() { return darkMode ? dp_bg_dark() : lv_color_hex(0xF8FAFC); }
static lv_color_t dp_card_bg() { return darkMode ? dp_card_dark() : lv_color_hex(0xFFFFFF); }
static lv_color_t dp_subcard_bg() { return darkMode ? dp_subcard_dark() : lv_color_hex(0xF1F5F9); }
static lv_color_t dp_border() { return darkMode ? lv_color_hex(0x3A4248) : lv_color_hex(0xE2E8F0); }

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
 * Crear sub-card estilo servidor (bg-white/5 rounded-lg)
 */
static lv_obj_t* createDPSubCard(lv_obj_t *parent, int x, int y, int w, int h) {
    lv_obj_t *sc = lv_obj_create(parent);
    lv_obj_set_pos(sc, x, y);
    lv_obj_set_size(sc, w, h);
    lv_obj_set_style_bg_color(sc, dp_subcard_bg(), 0);  // Tono 3: gris más claro
    lv_obj_set_style_bg_opa(sc, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(sc, 10, 0);  // rounded-lg
    lv_obj_set_style_border_width(sc, 0, 0);
    lv_obj_set_style_pad_all(sc, 10, 0);
    lv_obj_clear_flag(sc, LV_OBJ_FLAG_SCROLLABLE);
    return sc;
}

/**
 * Crear card principal estilo glassmorphism del servidor
 */
static lv_obj_t* createGlassCard(lv_obj_t *parent, int x, int y, int w, int h, const char* title) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_set_style_bg_color(card, dp_card_bg(), 0);  // Tono 2: gris oscuro
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(card, 16, 0);  // rounded-2xl
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, dp_border(), 0);
    lv_obj_set_style_shadow_width(card, 30, 0);
    lv_obj_set_style_shadow_color(card, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(card, darkMode ? LV_OPA_40 : LV_OPA_10, 0);
    lv_obj_set_style_shadow_ofs_y(card, 8, 0);
    lv_obj_set_style_pad_all(card, 12, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    // Titulo estilo servidor: card-title
    lv_obj_t *lbl = lv_label_create(card);
    lv_label_set_text(lbl, title);
    lv_obj_set_style_text_color(lbl, dp_text_muted(), 0);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 0, 0);

    return card;
}

/**
 * Crear pantalla detalle principal - Layout mejorado sin encimamientos
 */
void createDetailPrincipal() {
    extern WeatherData g_weather;
    extern CompareData g_compare;
    extern AlmanacData g_almanac;

    lv_obj_t *scr = lv_obj_create(NULL);
    detail_principal_ui.screen = scr;
    lv_obj_set_style_bg_color(scr, dp_bg(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(scr, 0, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    // === HEADER ===
    lv_obj_t *header = lv_obj_create(scr);
    lv_obj_set_size(header, SCREEN_WIDTH, 56);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, dp_card_bg(), 0);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_border_width(header, 1, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(header, dp_border(), 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_set_size(btn_back, 90, 32);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 12, 12);
    lv_obj_set_style_bg_color(btn_back, dp_subcard_bg(), 0);
    lv_obj_set_style_radius(btn_back, 6, 0);
    lv_obj_set_style_shadow_width(btn_back, 0, 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) { navigateToScreenById(0); }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_lbl = lv_label_create(btn_back);
    lv_label_set_text(btn_lbl, LV_SYMBOL_LEFT " Volver");
    lv_obj_set_style_text_color(btn_lbl, dp_text_primary(), 0);
    lv_obj_center(btn_lbl);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "ESTACION PRINCIPAL");
    lv_obj_set_style_text_color(title, dp_text_primary(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    lv_obj_t *subtitle = lv_label_create(scr);
    lv_label_set_text(subtitle, "Ecowitt WS2910 + WS69");
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0x60A5FA), 0);
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_12, 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 32);

    // Layout: usar toda la altura disponible (600px total)
    // Header: 56px, gap: 8px, footer zone: ~50px
    int gap = 8;
    int y = 62;
    int row1_h = 260;  // Primera fila
    int row2_h = 195;  // Segunda fila (reducida para evitar overflow)
    int col1_w = 340;
    int col2_w = 340;
    int col3_w = 316;

    // === CARD 1: TEMPERATURA ===
    lv_obj_t *card_temp = createGlassCard(scr, gap, y, col1_w, row1_h, "TEMPERATURA");

    // Sub-card 1: Temperatura actual
    lv_obj_t *sc_temp = createDPSubCard(card_temp, 0, 18, col1_w - 24, 90);

    lv_obj_t *temp_icon = lv_label_create(sc_temp);
    lv_label_set_text(temp_icon, WI_THERMOMETER);
    lv_obj_set_style_text_font(temp_icon, &weather_icons_48, 0);
    lv_obj_set_style_text_color(temp_icon, lv_color_hex(0xFCD34D), 0);
    lv_obj_align(temp_icon, LV_ALIGN_LEFT_MID, 5, 0);

    detail_principal_ui.temp_value = lv_label_create(sc_temp);
    lv_label_set_text(detail_principal_ui.temp_value, "--.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.temp_value, lv_color_hex(0xFCD34D), 0);
    lv_obj_set_style_text_font(detail_principal_ui.temp_value, &lv_font_montserrat_48, 0);
    lv_obj_align(detail_principal_ui.temp_value, LV_ALIGN_LEFT_MID, 70, 0);

    // Sub-card 2: Sensación y Rocío
    lv_obj_t *sc_temp_derived = createDPSubCard(card_temp, 0, 115, col1_w - 24, 45);

    detail_principal_ui.feels_like = lv_label_create(sc_temp_derived);
    lv_label_set_text(detail_principal_ui.feels_like, "Sensacion: --.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.feels_like, lv_color_hex(0xFB923C), 0);
    lv_obj_set_style_text_font(detail_principal_ui.feels_like, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.feels_like, LV_ALIGN_LEFT_MID, 10, 0);

    detail_principal_ui.dew_point = lv_label_create(sc_temp_derived);
    lv_label_set_text(detail_principal_ui.dew_point, "Rocio: --.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.dew_point, lv_color_hex(0x6EE7B7), 0);
    lv_obj_set_style_text_font(detail_principal_ui.dew_point, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.dew_point, LV_ALIGN_RIGHT_MID, -10, 0);

    // Sub-card 3: Min/Max/Compare
    lv_obj_t *sc_temp_stats = createDPSubCard(card_temp, 0, 167, col1_w - 24, 70);

    detail_principal_ui.temp_max = lv_label_create(sc_temp_stats);
    lv_label_set_text(detail_principal_ui.temp_max, LV_SYMBOL_UP " Max: --.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.temp_max, DP_COLOR_TEMP, 0);
    lv_obj_set_style_text_font(detail_principal_ui.temp_max, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.temp_max, LV_ALIGN_TOP_LEFT, 10, 8);

    detail_principal_ui.temp_min = lv_label_create(sc_temp_stats);
    lv_label_set_text(detail_principal_ui.temp_min, LV_SYMBOL_DOWN " Min: --.-°C");
    lv_obj_set_style_text_color(detail_principal_ui.temp_min, lv_color_hex(0x38BDF8), 0);
    lv_obj_set_style_text_font(detail_principal_ui.temp_min, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.temp_min, LV_ALIGN_TOP_RIGHT, -10, 8);

    detail_principal_ui.temp_compare = lv_label_create(sc_temp_stats);
    lv_label_set_text(detail_principal_ui.temp_compare, "vs ayer: --");
    lv_obj_set_style_text_color(detail_principal_ui.temp_compare, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.temp_compare, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.temp_compare, LV_ALIGN_BOTTOM_MID, 0, -8);

    // === CARD 2: HUMEDAD / PRESION ===
    lv_obj_t *card_hp = createGlassCard(scr, gap + col1_w + gap, y, col2_w, row1_h, "HUMEDAD / PRESION");

    // Sub-cards lado a lado para valores principales
    int hp_sc_w = (col2_w - 24 - 10) / 2;

    // Sub-card Humedad
    lv_obj_t *sc_hum = createDPSubCard(card_hp, 0, 18, hp_sc_w, 90);

    lv_obj_t *hum_icon = lv_label_create(sc_hum);
    lv_label_set_text(hum_icon, WI_HUMIDITY);
    lv_obj_set_style_text_font(hum_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(hum_icon, lv_color_hex(0x67E8F9), 0);
    lv_obj_align(hum_icon, LV_ALIGN_TOP_MID, 0, 5);

    detail_principal_ui.humidity = lv_label_create(sc_hum);
    lv_label_set_text(detail_principal_ui.humidity, "--%");
    lv_obj_set_style_text_color(detail_principal_ui.humidity, lv_color_hex(0x67E8F9), 0);
    lv_obj_set_style_text_font(detail_principal_ui.humidity, &lv_font_montserrat_28, 0);
    lv_obj_align(detail_principal_ui.humidity, LV_ALIGN_CENTER, 0, 12);

    lv_obj_t *hum_lbl = lv_label_create(sc_hum);
    lv_label_set_text(hum_lbl, "Humedad");
    lv_obj_set_style_text_font(hum_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(hum_lbl, dp_text_muted(), 0);
    lv_obj_align(hum_lbl, LV_ALIGN_BOTTOM_MID, 0, -5);

    // Sub-card Presion
    lv_obj_t *sc_pres = createDPSubCard(card_hp, hp_sc_w + 10, 18, hp_sc_w, 90);

    lv_obj_t *pres_icon = lv_label_create(sc_pres);
    lv_label_set_text(pres_icon, WI_BAROMETER);
    lv_obj_set_style_text_font(pres_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(pres_icon, lv_color_hex(0xC4B5FD), 0);
    lv_obj_align(pres_icon, LV_ALIGN_TOP_MID, 0, 5);

    detail_principal_ui.pressure = lv_label_create(sc_pres);
    lv_label_set_text(detail_principal_ui.pressure, "---- hPa");
    lv_obj_set_style_text_color(detail_principal_ui.pressure, lv_color_hex(0xC4B5FD), 0);
    lv_obj_set_style_text_font(detail_principal_ui.pressure, &lv_font_montserrat_20, 0);
    lv_obj_align(detail_principal_ui.pressure, LV_ALIGN_CENTER, 0, 12);

    lv_obj_t *pres_lbl = lv_label_create(sc_pres);
    lv_label_set_text(pres_lbl, "Presion");
    lv_obj_set_style_text_font(pres_lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(pres_lbl, dp_text_muted(), 0);
    lv_obj_align(pres_lbl, LV_ALIGN_BOTTOM_MID, 0, -5);

    // Sub-card Tendencia y Pronóstico
    lv_obj_t *sc_hp_info = createDPSubCard(card_hp, 0, 115, col2_w - 24, 120);

    detail_principal_ui.pressure_trend = lv_label_create(sc_hp_info);
    lv_label_set_text(detail_principal_ui.pressure_trend, "Tendencia: --");
    lv_obj_set_style_text_color(detail_principal_ui.pressure_trend, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.pressure_trend, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.pressure_trend, LV_ALIGN_TOP_LEFT, 10, 10);

    detail_principal_ui.forecast_text = lv_label_create(sc_hp_info);
    lv_label_set_text(detail_principal_ui.forecast_text, "Pronostico: --");
    lv_obj_set_style_text_color(detail_principal_ui.forecast_text, lv_color_hex(0x4ADE80), 0);
    lv_obj_set_style_text_font(detail_principal_ui.forecast_text, &lv_font_montserrat_12, 0);
    lv_obj_set_width(detail_principal_ui.forecast_text, col2_w - 50);
    lv_label_set_long_mode(detail_principal_ui.forecast_text, LV_LABEL_LONG_WRAP);
    lv_obj_align(detail_principal_ui.forecast_text, LV_ALIGN_TOP_LEFT, 10, 35);

    // === CARD 3: VIENTO ===
    lv_obj_t *card_wind = createGlassCard(scr, gap + col1_w + gap + col2_w + gap, y, col3_w, row1_h, "VIENTO");

    // Sub-card 1: Velocidad
    lv_obj_t *sc_wind = createDPSubCard(card_wind, 0, 18, col3_w - 24, 70);

    lv_obj_t *wind_icon = lv_label_create(sc_wind);
    lv_label_set_text(wind_icon, WI_STRONG_WIND);
    lv_obj_set_style_text_font(wind_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(wind_icon, lv_color_hex(0x6EE7B7), 0);
    lv_obj_align(wind_icon, LV_ALIGN_LEFT_MID, 10, 0);

    detail_principal_ui.wind_speed = lv_label_create(sc_wind);
    lv_label_set_text(detail_principal_ui.wind_speed, "-- km/h");
    lv_obj_set_style_text_color(detail_principal_ui.wind_speed, lv_color_hex(0x6EE7B7), 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_speed, &lv_font_montserrat_28, 0);
    lv_obj_align(detail_principal_ui.wind_speed, LV_ALIGN_LEFT_MID, 55, 0);

    // Sub-card 2: Dirección
    lv_obj_t *sc_wind_dir = createDPSubCard(card_wind, 0, 95, col3_w - 24, 45);

    lv_obj_t *dir_icon = lv_label_create(sc_wind_dir);
    lv_label_set_text(dir_icon, WI_DIR_N);
    lv_obj_set_style_text_font(dir_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(dir_icon, lv_color_hex(0x60A5FA), 0);
    lv_obj_align(dir_icon, LV_ALIGN_LEFT_MID, 10, 0);

    detail_principal_ui.wind_dir = lv_label_create(sc_wind_dir);
    lv_label_set_text(detail_principal_ui.wind_dir, "-- ---°");
    lv_obj_set_style_text_color(detail_principal_ui.wind_dir, lv_color_hex(0x60A5FA), 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_dir, &lv_font_montserrat_18, 0);
    lv_obj_align(detail_principal_ui.wind_dir, LV_ALIGN_LEFT_MID, 50, 0);

    // Sub-card 3: Estadísticas (Rafagas, Max, Beaufort)
    lv_obj_t *sc_wind_stats = createDPSubCard(card_wind, 0, 147, col3_w - 24, 90);

    detail_principal_ui.wind_gust = lv_label_create(sc_wind_stats);
    lv_label_set_text(detail_principal_ui.wind_gust, "Rafagas: -- km/h");
    lv_obj_set_style_text_color(detail_principal_ui.wind_gust, lv_color_hex(0xFDE047), 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_gust, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.wind_gust, LV_ALIGN_TOP_LEFT, 10, 8);

    detail_principal_ui.wind_max = lv_label_create(sc_wind_stats);
    lv_label_set_text(detail_principal_ui.wind_max, "Max hoy: -- km/h");
    lv_obj_set_style_text_color(detail_principal_ui.wind_max, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_max, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.wind_max, LV_ALIGN_TOP_LEFT, 10, 32);

    detail_principal_ui.wind_beaufort = lv_label_create(sc_wind_stats);
    lv_label_set_text(detail_principal_ui.wind_beaufort, "--");
    lv_obj_set_style_text_color(detail_principal_ui.wind_beaufort, lv_color_hex(0x6EE7B7), 0);
    lv_obj_set_style_text_font(detail_principal_ui.wind_beaufort, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.wind_beaufort, LV_ALIGN_TOP_LEFT, 10, 56);

    // === FILA 2 ===
    int y2 = y + row1_h + gap;
    int row2_w = (SCREEN_WIDTH - gap * 5) / 4;

    // Card LLUVIA
    lv_obj_t *card_rain = createGlassCard(scr, gap, y2, row2_w, row2_h, "PRECIPITACION");

    // Sub-card 1: Rate actual
    lv_obj_t *sc_rain = createDPSubCard(card_rain, 0, 18, row2_w - 24, 50);
    lv_obj_t *rain_icon = lv_label_create(sc_rain);
    lv_label_set_text(rain_icon, WI_RAIN);
    lv_obj_set_style_text_font(rain_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(rain_icon, lv_color_hex(0x60A5FA), 0);
    lv_obj_align(rain_icon, LV_ALIGN_LEFT_MID, 5, 0);

    detail_principal_ui.rain_rate = lv_label_create(sc_rain);
    lv_label_set_text(detail_principal_ui.rain_rate, "-.-- mm/h");
    lv_obj_set_style_text_color(detail_principal_ui.rain_rate, lv_color_hex(0x60A5FA), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_rate, &lv_font_montserrat_18, 0);
    lv_obj_align(detail_principal_ui.rain_rate, LV_ALIGN_LEFT_MID, 45, 0);

    // Sub-card 2: Acumulados
    lv_obj_t *sc_rain_acc = createDPSubCard(card_rain, 0, 75, row2_w - 24, 100);

    detail_principal_ui.rain_day = lv_label_create(sc_rain_acc);
    lv_label_set_text(detail_principal_ui.rain_day, "Hoy: -- mm");
    lv_obj_set_style_text_color(detail_principal_ui.rain_day, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_day, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_principal_ui.rain_day, LV_ALIGN_TOP_LEFT, 10, 8);

    detail_principal_ui.rain_week = lv_label_create(sc_rain_acc);
    lv_label_set_text(detail_principal_ui.rain_week, "Semana: -- mm");
    lv_obj_set_style_text_color(detail_principal_ui.rain_week, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_week, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_principal_ui.rain_week, LV_ALIGN_TOP_LEFT, 10, 30);

    detail_principal_ui.rain_month = lv_label_create(sc_rain_acc);
    lv_label_set_text(detail_principal_ui.rain_month, "Mes: -- mm");
    lv_obj_set_style_text_color(detail_principal_ui.rain_month, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_month, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_principal_ui.rain_month, LV_ALIGN_TOP_LEFT, 10, 52);

    detail_principal_ui.rain_year = lv_label_create(sc_rain_acc);
    lv_label_set_text(detail_principal_ui.rain_year, "Anual: -- mm");
    lv_obj_set_style_text_color(detail_principal_ui.rain_year, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.rain_year, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_principal_ui.rain_year, LV_ALIGN_TOP_LEFT, 10, 74);

    // Card UV/SOLAR
    lv_obj_t *card_uv = createGlassCard(scr, gap + row2_w + gap, y2, row2_w, row2_h, "UV / SOLAR");

    // Sub-card 1: Indice UV
    lv_obj_t *sc_uv = createDPSubCard(card_uv, 0, 18, row2_w - 24, 55);
    detail_principal_ui.uv_index = lv_label_create(sc_uv);
    lv_label_set_text(detail_principal_ui.uv_index, "UV: --");
    lv_obj_set_style_text_color(detail_principal_ui.uv_index, lv_color_hex(0xFCD34D), 0);
    lv_obj_set_style_text_font(detail_principal_ui.uv_index, &lv_font_montserrat_24, 0);
    lv_obj_align(detail_principal_ui.uv_index, LV_ALIGN_LEFT_MID, 10, 0);

    detail_principal_ui.uv_level = lv_label_create(sc_uv);
    lv_label_set_text(detail_principal_ui.uv_level, "(---)");
    lv_obj_set_style_text_color(detail_principal_ui.uv_level, lv_color_hex(0xFCD34D), 0);
    lv_obj_set_style_text_font(detail_principal_ui.uv_level, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.uv_level, LV_ALIGN_RIGHT_MID, -10, 0);

    // Sub-card 2: Radiación solar
    lv_obj_t *sc_solar = createDPSubCard(card_uv, 0, 80, row2_w - 24, 40);
    detail_principal_ui.solar_radiation = lv_label_create(sc_solar);
    lv_label_set_text(detail_principal_ui.solar_radiation, "Radiacion: -- W/m2");
    lv_obj_set_style_text_color(detail_principal_ui.solar_radiation, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.solar_radiation, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.solar_radiation, LV_ALIGN_LEFT_MID, 10, 0);

    // Card AQI
    lv_obj_t *card_aqi = createGlassCard(scr, gap + (row2_w + gap) * 2, y2, row2_w, row2_h, "CALIDAD AIRE");

    lv_obj_t *sc_aqi = createDPSubCard(card_aqi, 0, 18, row2_w - 24, 70);

    detail_principal_ui.aqi_indicator = lv_obj_create(sc_aqi);
    lv_obj_set_size(detail_principal_ui.aqi_indicator, 45, 45);
    lv_obj_align(detail_principal_ui.aqi_indicator, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_set_style_bg_color(detail_principal_ui.aqi_indicator, lv_color_hex(0x4ADE80), 0);
    lv_obj_set_style_radius(detail_principal_ui.aqi_indicator, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(detail_principal_ui.aqi_indicator, 0, 0);
    lv_obj_clear_flag(detail_principal_ui.aqi_indicator, LV_OBJ_FLAG_SCROLLABLE);

    detail_principal_ui.aqi_value = lv_label_create(sc_aqi);
    lv_label_set_text(detail_principal_ui.aqi_value, "AQI: --");
    lv_obj_set_style_text_color(detail_principal_ui.aqi_value, dp_text_primary(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.aqi_value, &lv_font_montserrat_20, 0);
    lv_obj_align(detail_principal_ui.aqi_value, LV_ALIGN_LEFT_MID, 60, -8);

    detail_principal_ui.aqi_level = lv_label_create(sc_aqi);
    lv_label_set_text(detail_principal_ui.aqi_level, "---");
    lv_obj_set_style_text_color(detail_principal_ui.aqi_level, lv_color_hex(0x4ADE80), 0);
    lv_obj_set_style_text_font(detail_principal_ui.aqi_level, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_principal_ui.aqi_level, LV_ALIGN_LEFT_MID, 60, 12);

    // Card SOL/LUNA con sub-cards separadas
    lv_obj_t *card_sun = createGlassCard(scr, gap + (row2_w + gap) * 3, y2, row2_w, row2_h, "SOL / LUNA");

    // Sub-card Amanecer
    lv_obj_t *sc_sunrise = createDPSubCard(card_sun, 0, 18, row2_w - 24, 40);
    lv_obj_t *sun_icon = lv_label_create(sc_sunrise);
    lv_label_set_text(sun_icon, WI_SUNRISE);
    lv_obj_set_style_text_font(sun_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(sun_icon, lv_color_hex(0xFFB800), 0);
    lv_obj_align(sun_icon, LV_ALIGN_LEFT_MID, 5, 0);

    detail_principal_ui.sunrise = lv_label_create(sc_sunrise);
    lv_label_set_text(detail_principal_ui.sunrise, "--:--");
    lv_obj_set_style_text_color(detail_principal_ui.sunrise, lv_color_hex(0xFFB800), 0);
    lv_obj_set_style_text_font(detail_principal_ui.sunrise, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.sunrise, LV_ALIGN_LEFT_MID, 48, 0);

    // Sub-card Atardecer
    lv_obj_t *sc_sunset = createDPSubCard(card_sun, 0, 62, row2_w - 24, 40);
    lv_obj_t *set_icon = lv_label_create(sc_sunset);
    lv_label_set_text(set_icon, WI_SUNSET);
    lv_obj_set_style_text_font(set_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(set_icon, lv_color_hex(0xFB923C), 0);
    lv_obj_align(set_icon, LV_ALIGN_LEFT_MID, 5, 0);

    detail_principal_ui.sunset = lv_label_create(sc_sunset);
    lv_label_set_text(detail_principal_ui.sunset, "--:--");
    lv_obj_set_style_text_color(detail_principal_ui.sunset, lv_color_hex(0xFB923C), 0);
    lv_obj_set_style_text_font(detail_principal_ui.sunset, &lv_font_montserrat_14, 0);
    lv_obj_align(detail_principal_ui.sunset, LV_ALIGN_LEFT_MID, 48, 0);

    // Sub-card Luna
    lv_obj_t *sc_moon = createDPSubCard(card_sun, 0, 106, row2_w - 24, 55);
    detail_principal_ui.moon_icon = lv_label_create(sc_moon);
    lv_label_set_text(detail_principal_ui.moon_icon, WI_MOON_FULL);
    lv_obj_set_style_text_font(detail_principal_ui.moon_icon, &weather_icons_32, 0);
    lv_obj_set_style_text_color(detail_principal_ui.moon_icon, lv_color_hex(0xE8EDF5), 0);
    lv_obj_align(detail_principal_ui.moon_icon, LV_ALIGN_LEFT_MID, 5, 0);

    detail_principal_ui.moon_phase = lv_label_create(sc_moon);
    lv_label_set_text(detail_principal_ui.moon_phase, "---");
    lv_obj_set_style_text_color(detail_principal_ui.moon_phase, lv_color_hex(0xE8EDF5), 0);
    lv_obj_set_style_text_font(detail_principal_ui.moon_phase, &lv_font_montserrat_14, 0);  // Mismo tamaño que amanecer
    lv_obj_align(detail_principal_ui.moon_phase, LV_ALIGN_LEFT_MID, 48, -10);

    detail_principal_ui.moon_illum = lv_label_create(sc_moon);
    lv_label_set_text(detail_principal_ui.moon_illum, "Ilum: --%");
    lv_obj_set_style_text_color(detail_principal_ui.moon_illum, dp_text_muted(), 0);
    lv_obj_set_style_text_font(detail_principal_ui.moon_illum, &lv_font_montserrat_14, 0);  // Mismo tamaño
    lv_obj_align(detail_principal_ui.moon_illum, LV_ALIGN_LEFT_MID, 48, 10);

    // Barra de baterias al fondo
    lv_obj_t *batt_bar = lv_obj_create(scr);
    lv_obj_set_size(batt_bar, SCREEN_WIDTH - 20, 24);
    lv_obj_align(batt_bar, LV_ALIGN_BOTTOM_MID, 0, -22);
    lv_obj_set_style_bg_color(batt_bar, dp_subcard_bg(), 0);
    lv_obj_set_style_bg_opa(batt_bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(batt_bar, 6, 0);
    lv_obj_set_style_border_width(batt_bar, 0, 0);
    lv_obj_clear_flag(batt_bar, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *batt_title = lv_label_create(batt_bar);
    lv_label_set_text(batt_title, LV_SYMBOL_BATTERY_FULL " BATERIAS:");
    lv_obj_set_style_text_color(batt_title, dp_text_muted(), 0);
    lv_obj_set_style_text_font(batt_title, &lv_font_montserrat_12, 0);
    lv_obj_align(batt_title, LV_ALIGN_LEFT_MID, 10, 0);

    detail_principal_ui.battery_ws69 = lv_label_create(batt_bar);
    lv_label_set_text(detail_principal_ui.battery_ws69, "WS69: --");
    lv_obj_set_style_text_color(detail_principal_ui.battery_ws69, lv_color_hex(0x4ADE80), 0);
    lv_obj_set_style_text_font(detail_principal_ui.battery_ws69, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_principal_ui.battery_ws69, LV_ALIGN_LEFT_MID, 140, 0);

    detail_principal_ui.battery_wn31 = lv_label_create(batt_bar);
    lv_label_set_text(detail_principal_ui.battery_wn31, "WN31: --");
    lv_obj_set_style_text_color(detail_principal_ui.battery_wn31, lv_color_hex(0x4ADE80), 0);
    lv_obj_set_style_text_font(detail_principal_ui.battery_wn31, &lv_font_montserrat_12, 0);
    lv_obj_align(detail_principal_ui.battery_wn31, LV_ALIGN_LEFT_MID, 280, 0);

    lv_obj_t *footer = lv_label_create(scr);
    lv_label_set_text(footer, LV_SYMBOL_LEFT " Swipe " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(footer, dp_text_muted(), 0);
    lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
    lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -6);

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

    snprintf(buf, sizeof(buf), "Sensacion: %.1f°C", g_weather.feels_like);
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
            snprintf(buf, sizeof(buf), "Tendencia: " LV_SYMBOL_UP " +%.1f hPa", g_compare.pressure_diff);
            lv_obj_set_style_text_color(detail_principal_ui.pressure_trend, lv_color_hex(0x4ADE80), 0);
        } else if (g_compare.pressure_diff < -0.5) {
            snprintf(buf, sizeof(buf), "Tendencia: " LV_SYMBOL_DOWN " %.1f hPa", g_compare.pressure_diff);
            lv_obj_set_style_text_color(detail_principal_ui.pressure_trend, lv_color_hex(0xF87171), 0);
        } else {
            snprintf(buf, sizeof(buf), "Tendencia: " LV_SYMBOL_RIGHT " Estable");
            lv_obj_set_style_text_color(detail_principal_ui.pressure_trend, dp_text_muted(), 0);
        }
        lv_label_set_text(detail_principal_ui.pressure_trend, buf);
    } else {
        lv_label_set_text(detail_principal_ui.pressure_trend, "Tendencia: --");
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

    snprintf(buf, sizeof(buf), "Direccion: %s %.0f°", g_weather.wind_dir_cardinal, g_weather.wind_dir);
    lv_label_set_text(detail_principal_ui.wind_dir, buf);

    snprintf(buf, sizeof(buf), "Rafagas: %.0f km/h", g_weather.wind_gust);
    lv_label_set_text(detail_principal_ui.wind_gust, buf);

    if (g_stats.valid && strlen(g_stats.wind_max_time) > 0) {
        snprintf(buf, sizeof(buf), "Maximo: %.0f km/h (%s)", g_stats.wind_max, g_stats.wind_max_time);
    } else {
        snprintf(buf, sizeof(buf), "Maximo: %.0f km/h", g_weather.wind_max);
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

    snprintf(buf, sizeof(buf), "Radiacion Solar: %.0f W/m2", g_weather.solar_radiation);
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

        // Icono de luna dinamico segun fase
        bool is_waxing = (strstr(g_almanac.moon_phase, "waxing") != nullptr ||
                          strstr(g_almanac.moon_phase, "creciente") != nullptr ||
                          strstr(g_almanac.moon_phase, "Waxing") != nullptr ||
                          strstr(g_almanac.moon_phase, "Creciente") != nullptr);
        const char* moon_icon = getMoonPhaseIcon(g_almanac.moon_illumination, is_waxing);
        lv_label_set_text(detail_principal_ui.moon_icon, moon_icon);
    }

    // Baterias
    extern RemoteSensorData g_jardin;

    // WS69 (estacion exterior) - battery_wh65 es booleano
    if (g_weather.valid) {
        if (g_weather.battery_wh65) {
            lv_label_set_text(detail_principal_ui.battery_ws69, "WS69: " LV_SYMBOL_OK " OK");
            lv_obj_set_style_text_color(detail_principal_ui.battery_ws69, lv_color_hex(0x4ADE80), 0);
        } else {
            lv_label_set_text(detail_principal_ui.battery_ws69, "WS69: " LV_SYMBOL_WARNING " Baja!");
            lv_obj_set_style_text_color(detail_principal_ui.battery_ws69, lv_color_hex(0xF87171), 0);
        }
    }

    // WN31 (Jardin) - battery es 100 (OK) o 10 (baja)
    if (g_jardin.valid) {
        if (g_jardin.battery > 50) {
            lv_label_set_text(detail_principal_ui.battery_wn31, "WN31 (Jardin): " LV_SYMBOL_OK " OK");
            lv_obj_set_style_text_color(detail_principal_ui.battery_wn31, lv_color_hex(0x4ADE80), 0);
        } else {
            lv_label_set_text(detail_principal_ui.battery_wn31, "WN31 (Jardin): " LV_SYMBOL_WARNING " Baja!");
            lv_obj_set_style_text_color(detail_principal_ui.battery_wn31, lv_color_hex(0xF87171), 0);
        }
    } else {
        lv_label_set_text(detail_principal_ui.battery_wn31, "WN31 (Jardin): --");
        lv_obj_set_style_text_color(detail_principal_ui.battery_wn31, dp_text_muted(), 0);
    }
}

/**
 * Obtener referencia a la pantalla
 */
lv_obj_t* getDetailPrincipalScreen() {
    return detail_principal_ui.screen;
}

#endif // UI_DETAIL_PRINCIPAL_H
