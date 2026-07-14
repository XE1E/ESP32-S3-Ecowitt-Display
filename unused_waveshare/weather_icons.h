/**
 * weather_icons.h - Weather Icon Drawing
 *
 * Draws weather condition icons using LVGL canvas
 * Based on WeatherAPI condition codes
 */

#ifndef WEATHER_ICONS_H
#define WEATHER_ICONS_H

#include <Arduino.h>
#include <lvgl.h>

// ============================================================================
// Weather Icon Types
// ============================================================================

enum WeatherIconType {
    ICON_SUNNY = 0,
    ICON_CLEAR_NIGHT,
    ICON_PARTLY_CLOUDY,
    ICON_PARTLY_CLOUDY_NIGHT,
    ICON_CLOUDY,
    ICON_OVERCAST,
    ICON_FOG,
    ICON_RAIN_LIGHT,
    ICON_RAIN_HEAVY,
    ICON_THUNDERSTORM,
    ICON_SNOW,
    ICON_SLEET,
    ICON_UNKNOWN
};

// ============================================================================
// Color Definitions
// ============================================================================

#define COLOR_SUN       lv_color_hex(0xFFD93D)
#define COLOR_SUN_GLOW  lv_color_hex(0xFFF176)
#define COLOR_MOON      lv_color_hex(0xE8E8E8)
#define COLOR_CLOUD     lv_color_hex(0xB0BEC5)
#define COLOR_CLOUD_DARK lv_color_hex(0x78909C)
#define COLOR_RAIN      lv_color_hex(0x64B5F6)
#define COLOR_SNOW      lv_color_hex(0xE3F2FD)
#define COLOR_LIGHTNING lv_color_hex(0xFFEB3B)

// ============================================================================
// Map Condition Code to Icon Type
// ============================================================================

WeatherIconType getIconTypeFromCode(int code, bool is_day) {
    switch (code) {
        // Clear
        case 1000:
            return is_day ? ICON_SUNNY : ICON_CLEAR_NIGHT;

        // Partly cloudy
        case 1003:
            return is_day ? ICON_PARTLY_CLOUDY : ICON_PARTLY_CLOUDY_NIGHT;

        // Cloudy
        case 1006:
            return ICON_CLOUDY;

        // Overcast
        case 1009:
            return ICON_OVERCAST;

        // Mist, Fog, Freezing fog
        case 1030:
        case 1135:
        case 1147:
            return ICON_FOG;

        // Light rain, drizzle
        case 1063:  // Patchy rain
        case 1150:  // Patchy light drizzle
        case 1153:  // Light drizzle
        case 1180:  // Patchy light rain
        case 1183:  // Light rain
        case 1240:  // Light rain shower
            return ICON_RAIN_LIGHT;

        // Heavy rain
        case 1186:  // Moderate rain at times
        case 1189:  // Moderate rain
        case 1192:  // Heavy rain at times
        case 1195:  // Heavy rain
        case 1243:  // Moderate or heavy rain shower
        case 1246:  // Torrential rain shower
            return ICON_RAIN_HEAVY;

        // Snow
        case 1066:  // Patchy snow
        case 1114:  // Blowing snow
        case 1210:  // Patchy light snow
        case 1213:  // Light snow
        case 1216:  // Patchy moderate snow
        case 1219:  // Moderate snow
        case 1222:  // Patchy heavy snow
        case 1225:  // Heavy snow
        case 1255:  // Light snow showers
        case 1258:  // Moderate or heavy snow showers
            return ICON_SNOW;

        // Sleet
        case 1069:  // Patchy sleet
        case 1072:  // Patchy freezing drizzle
        case 1168:  // Freezing drizzle
        case 1171:  // Heavy freezing drizzle
        case 1198:  // Light freezing rain
        case 1201:  // Moderate or heavy freezing rain
        case 1204:  // Light sleet
        case 1207:  // Moderate or heavy sleet
        case 1237:  // Ice pellets
        case 1249:  // Light sleet showers
        case 1252:  // Moderate or heavy sleet showers
        case 1261:  // Light showers of ice pellets
        case 1264:  // Moderate or heavy showers of ice pellets
            return ICON_SLEET;

        // Thunderstorm
        case 1087:  // Thundery outbreaks
        case 1273:  // Patchy light rain with thunder
        case 1276:  // Moderate or heavy rain with thunder
        case 1279:  // Patchy light snow with thunder
        case 1282:  // Moderate or heavy snow with thunder
            return ICON_THUNDERSTORM;

        default:
            return ICON_UNKNOWN;
    }
}

// ============================================================================
// Draw Sun
// ============================================================================

void drawSun(lv_obj_t *canvas, int cx, int cy, int radius) {
    // Sun glow (larger circle, semi-transparent)
    lv_draw_arc_dsc_t glow_dsc;
    lv_draw_arc_dsc_init(&glow_dsc);
    glow_dsc.color = COLOR_SUN_GLOW;
    glow_dsc.width = radius / 2;
    glow_dsc.opa = LV_OPA_50;
    lv_canvas_draw_arc(canvas, cx, cy, radius + 5, 0, 360, &glow_dsc);

    // Sun body
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = COLOR_SUN;
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.radius = LV_RADIUS_CIRCLE;

    lv_canvas_draw_rect(canvas, cx - radius, cy - radius, radius * 2, radius * 2, &rect_dsc);

    // Sun rays
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = COLOR_SUN;
    line_dsc.width = 3;
    line_dsc.opa = LV_OPA_COVER;

    int ray_len = radius / 2;
    int ray_dist = radius + 8;
    for (int angle = 0; angle < 360; angle += 45) {
        float rad = angle * PI / 180;
        lv_point_t points[2];
        points[0].x = cx + cos(rad) * ray_dist;
        points[0].y = cy + sin(rad) * ray_dist;
        points[1].x = cx + cos(rad) * (ray_dist + ray_len);
        points[1].y = cy + sin(rad) * (ray_dist + ray_len);
        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
    }
}

// ============================================================================
// Draw Moon
// ============================================================================

void drawMoon(lv_obj_t *canvas, int cx, int cy, int radius) {
    // Moon body
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = COLOR_MOON;
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.radius = LV_RADIUS_CIRCLE;

    lv_canvas_draw_rect(canvas, cx - radius, cy - radius, radius * 2, radius * 2, &rect_dsc);

    // Moon shadow (crescent effect)
    rect_dsc.bg_color = lv_color_hex(0x1a1a2e);  // Background color
    lv_canvas_draw_rect(canvas, cx - radius + radius/2, cy - radius - 2,
                        radius * 2, radius * 2 + 4, &rect_dsc);
}

// ============================================================================
// Draw Cloud
// ============================================================================

void drawCloud(lv_obj_t *canvas, int cx, int cy, int size, bool dark) {
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = dark ? COLOR_CLOUD_DARK : COLOR_CLOUD;
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.radius = LV_RADIUS_CIRCLE;

    int r1 = size;
    int r2 = size * 0.7;
    int r3 = size * 0.6;

    // Main body (overlapping circles)
    lv_canvas_draw_rect(canvas, cx - r1, cy - r1/2, r1 * 2, r1, &rect_dsc);
    lv_canvas_draw_rect(canvas, cx - r1 - r2/2, cy - r2, r2 * 2, r2 * 2, &rect_dsc);
    lv_canvas_draw_rect(canvas, cx + r1/3, cy - r2 - r3/2, r3 * 2, r3 * 2, &rect_dsc);
    lv_canvas_draw_rect(canvas, cx - r1/2, cy - r1, r2 * 2, r2 * 2, &rect_dsc);
}

// ============================================================================
// Draw Rain Drops
// ============================================================================

void drawRainDrops(lv_obj_t *canvas, int cx, int cy, int width, int count, bool heavy) {
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = COLOR_RAIN;
    line_dsc.width = heavy ? 3 : 2;
    line_dsc.opa = LV_OPA_COVER;

    int drop_len = heavy ? 15 : 10;
    int spacing = width / count;

    for (int i = 0; i < count; i++) {
        int x = cx - width/2 + i * spacing + spacing/2;
        int y_offset = (i % 2) * 8;  // Stagger drops

        lv_point_t points[2];
        points[0].x = x;
        points[0].y = cy + y_offset;
        points[1].x = x - 3;
        points[1].y = cy + y_offset + drop_len;
        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
    }
}

// ============================================================================
// Draw Snow Flakes
// ============================================================================

void drawSnowflakes(lv_obj_t *canvas, int cx, int cy, int width, int count) {
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = COLOR_SNOW;
    rect_dsc.bg_opa = LV_OPA_COVER;
    rect_dsc.radius = LV_RADIUS_CIRCLE;

    int spacing = width / count;

    for (int i = 0; i < count; i++) {
        int x = cx - width/2 + i * spacing + spacing/2;
        int y_offset = (i % 3) * 6;
        int size = 3 + (i % 2) * 2;

        lv_canvas_draw_rect(canvas, x - size/2, cy + y_offset - size/2,
                            size, size, &rect_dsc);
    }
}

// ============================================================================
// Draw Lightning
// ============================================================================

void drawLightning(lv_obj_t *canvas, int cx, int cy) {
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = COLOR_LIGHTNING;
    line_dsc.width = 4;
    line_dsc.opa = LV_OPA_COVER;

    lv_point_t bolt[5];
    bolt[0].x = cx;     bolt[0].y = cy;
    bolt[1].x = cx - 8; bolt[1].y = cy + 15;
    bolt[2].x = cx + 3; bolt[2].y = cy + 15;
    bolt[3].x = cx - 5; bolt[3].y = cy + 30;
    bolt[4].x = cx + 5; bolt[4].y = cy + 12;

    for (int i = 0; i < 4; i++) {
        lv_point_t pts[2] = {bolt[i], bolt[i+1]};
        lv_canvas_draw_line(canvas, pts, 2, &line_dsc);
    }
}

// ============================================================================
// Create Weather Icon
// ============================================================================

lv_obj_t* createWeatherIcon(lv_obj_t *parent, int x, int y, int size,
                            int condition_code, bool is_day) {

    WeatherIconType iconType = getIconTypeFromCode(condition_code, is_day);

    // Create canvas
    lv_obj_t *canvas = lv_canvas_create(parent);
    lv_obj_set_pos(canvas, x, y);
    lv_obj_set_size(canvas, size, size);

    // Allocate buffer for canvas
    static lv_color_t *cbuf = nullptr;
    if (cbuf) free(cbuf);
    cbuf = (lv_color_t *)ps_malloc(size * size * sizeof(lv_color_t));

    if (!cbuf) {
        Serial.println("Failed to allocate canvas buffer");
        return canvas;
    }

    lv_canvas_set_buffer(canvas, cbuf, size, size, LV_IMG_CF_TRUE_COLOR_ALPHA);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x000000), LV_OPA_TRANSP);

    int cx = size / 2;
    int cy = size / 2;
    int r = size / 4;

    switch (iconType) {
        case ICON_SUNNY:
            drawSun(canvas, cx, cy, r);
            break;

        case ICON_CLEAR_NIGHT:
            drawMoon(canvas, cx, cy, r);
            break;

        case ICON_PARTLY_CLOUDY:
            drawSun(canvas, cx - r/2, cy - r/2, r * 0.8);
            drawCloud(canvas, cx + r/3, cy + r/3, r * 0.6, false);
            break;

        case ICON_PARTLY_CLOUDY_NIGHT:
            drawMoon(canvas, cx - r/2, cy - r/2, r * 0.7);
            drawCloud(canvas, cx + r/3, cy + r/3, r * 0.6, false);
            break;

        case ICON_CLOUDY:
            drawCloud(canvas, cx, cy, r, false);
            break;

        case ICON_OVERCAST:
            drawCloud(canvas, cx - r/4, cy - r/4, r * 0.7, false);
            drawCloud(canvas, cx + r/4, cy + r/4, r * 0.8, true);
            break;

        case ICON_FOG:
            drawCloud(canvas, cx, cy - r/2, r * 0.6, false);
            // Fog lines
            {
                lv_draw_line_dsc_t fog_dsc;
                lv_draw_line_dsc_init(&fog_dsc);
                fog_dsc.color = COLOR_CLOUD;
                fog_dsc.width = 3;
                fog_dsc.opa = LV_OPA_60;
                for (int i = 0; i < 3; i++) {
                    lv_point_t pts[2];
                    pts[0].x = cx - r;
                    pts[0].y = cy + r/2 + i * 8;
                    pts[1].x = cx + r;
                    pts[1].y = cy + r/2 + i * 8;
                    lv_canvas_draw_line(canvas, pts, 2, &fog_dsc);
                }
            }
            break;

        case ICON_RAIN_LIGHT:
            drawCloud(canvas, cx, cy - r/2, r * 0.8, false);
            drawRainDrops(canvas, cx, cy + r/2, r * 1.5, 3, false);
            break;

        case ICON_RAIN_HEAVY:
            drawCloud(canvas, cx, cy - r/2, r * 0.8, true);
            drawRainDrops(canvas, cx, cy + r/2, r * 1.5, 5, true);
            break;

        case ICON_THUNDERSTORM:
            drawCloud(canvas, cx, cy - r/2, r * 0.8, true);
            drawLightning(canvas, cx, cy);
            drawRainDrops(canvas, cx, cy + r/2 + 10, r, 3, true);
            break;

        case ICON_SNOW:
            drawCloud(canvas, cx, cy - r/2, r * 0.8, false);
            drawSnowflakes(canvas, cx, cy + r/2, r * 1.5, 5);
            break;

        case ICON_SLEET:
            drawCloud(canvas, cx, cy - r/2, r * 0.8, true);
            drawRainDrops(canvas, cx - r/3, cy + r/2, r * 0.5, 2, false);
            drawSnowflakes(canvas, cx + r/3, cy + r/2, r * 0.5, 2);
            break;

        default:
            // Unknown - draw question mark
            {
                lv_draw_label_dsc_t label_dsc;
                lv_draw_label_dsc_init(&label_dsc);
                label_dsc.color = lv_color_white();
                label_dsc.font = &lv_font_montserrat_28;
                lv_canvas_draw_text(canvas, cx - 10, cy - 15, 30, &label_dsc, "?");
            }
            break;
    }

    return canvas;
}

// ============================================================================
// Simple Icon Label (Fallback using symbols)
// ============================================================================

const char* getWeatherSymbol(int condition_code, bool is_day) {
    WeatherIconType iconType = getIconTypeFromCode(condition_code, is_day);

    switch (iconType) {
        case ICON_SUNNY:           return "O";   // Sun
        case ICON_CLEAR_NIGHT:     return "C";   // Moon crescent
        case ICON_PARTLY_CLOUDY:
        case ICON_PARTLY_CLOUDY_NIGHT:
        case ICON_CLOUDY:
        case ICON_OVERCAST:        return "C";  // Cloud
        case ICON_FOG:             return "=";   // Fog lines
        case ICON_RAIN_LIGHT:
        case ICON_RAIN_HEAVY:      return "'";   // Rain drops
        case ICON_THUNDERSTORM:    return "/";   // Lightning
        case ICON_SNOW:            return "*";   // Snowflake
        case ICON_SLEET:           return "~";   // Mixed
        default:                   return "?";
    }
}

// Get color for weather icon
lv_color_t getWeatherIconColor(int condition_code, bool is_day) {
    WeatherIconType iconType = getIconTypeFromCode(condition_code, is_day);

    switch (iconType) {
        case ICON_SUNNY:           return COLOR_SUN;
        case ICON_CLEAR_NIGHT:     return COLOR_MOON;
        case ICON_PARTLY_CLOUDY:
        case ICON_PARTLY_CLOUDY_NIGHT:
                                   return lv_color_hex(0xFFE082);  // Light yellow
        case ICON_CLOUDY:
        case ICON_OVERCAST:        return COLOR_CLOUD;
        case ICON_FOG:             return lv_color_hex(0x90A4AE);
        case ICON_RAIN_LIGHT:
        case ICON_RAIN_HEAVY:      return COLOR_RAIN;
        case ICON_THUNDERSTORM:    return COLOR_LIGHTNING;
        case ICON_SNOW:
        case ICON_SLEET:           return COLOR_SNOW;
        default:                   return lv_color_white();
    }
}

#endif // WEATHER_ICONS_H
