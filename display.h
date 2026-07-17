/**
 * display.h - Display initialization and LVGL setup
 *
 * Uses Arduino_GFX for Waveshare ESP32-S3-Touch-LCD-7B
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include "config.h"

// Arduino_GFX includes
#include <Arduino_GFX_Library.h>

// ============================================================================
// CH422G IO Expander (for backlight, touch reset, lcd reset)
// ============================================================================
#define CH422G_ADDR             0x24
#define CH422G_MODE_CMD         0x02
#define CH422G_OUTPUT_CMD       0x03
#define CH422G_PWM_CMD          0x05

// IO pins on CH422G
#define CH422G_TOUCH_RST    1
#define CH422G_BACKLIGHT    2
#define CH422G_LCD_RST      3

static uint8_t ch422g_output_state = 0xFF;

void ch422g_write_cmd(uint8_t cmd, uint8_t value) {
    Wire.beginTransmission(CH422G_ADDR);
    Wire.write(cmd);
    Wire.write(value);
    Wire.endTransmission();
}

void ch422g_init() {
    // Set all pins to output mode
    ch422g_write_cmd(CH422G_MODE_CMD, 0xFF);
    delay(10);

    // Initial state: all outputs high (resets inactive)
    ch422g_output_state = 0xFF;
    ch422g_write_cmd(CH422G_OUTPUT_CMD, ch422g_output_state);
    delay(10);
}

void ch422g_set_pin(uint8_t pin, bool state) {
    if (state) {
        ch422g_output_state |= (1 << pin);
    } else {
        ch422g_output_state &= ~(1 << pin);
    }
    ch422g_write_cmd(CH422G_OUTPUT_CMD, ch422g_output_state);
}

void ch422g_set_backlight_pwm(uint8_t brightness) {
    // brightness: 0-100
    if (brightness > 100) brightness = 100;
    if (brightness < 1) brightness = 1;

    // Logarithmic curve for perceptual linearity
    float normalized = brightness / 100.0f;
    float pwm_float = 255.0f * (pow(10.0f, normalized) - 1.0f) / 9.0f;
    uint8_t pwm_value = constrain((int)pwm_float, 3, 247);

    ch422g_write_cmd(CH422G_PWM_CMD, pwm_value);
}

void ch422g_backlight_on() {
    ch422g_set_backlight_pwm(80);  // 80% brightness
}

// ============================================================================
// Display Bus and Panel
// ============================================================================

Arduino_ESP32RGBPanel *rgbpanel = nullptr;
Arduino_RGB_Display *gfx = nullptr;

// LVGL display buffer (in PSRAM)
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1 = nullptr;
static lv_color_t *buf2 = nullptr;

// Display driver
static lv_disp_drv_t disp_drv;

// ============================================================================
// Display Flush Callback
// ============================================================================

void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);

    lv_disp_flush_ready(drv);
}

// ============================================================================
// Initialize Display
// ============================================================================

void initDisplay() {
    Serial.println("Initializing display...");

    // Initialize CH422G IO expander (controls backlight, resets)
    ch422g_init();
    Serial.println("CH422G initialized");

    // Create RGB panel bus (timings from Waveshare original rgb_lcd_port.cpp)
    rgbpanel = new Arduino_ESP32RGBPanel(
        LCD_DE, LCD_VSYNC, LCD_HSYNC, LCD_PCLK,
        LCD_R0, LCD_R1, LCD_R2, LCD_R3, LCD_R4,
        LCD_G0, LCD_G1, LCD_G2, LCD_G3, LCD_G4, LCD_G5,
        LCD_B0, LCD_B1, LCD_B2, LCD_B3, LCD_B4,
        0,      // hsync_polarity
        48,     // hsync_front_porch
        162,    // hsync_pulse_width
        152,    // hsync_back_porch
        0,      // vsync_polarity
        3,      // vsync_front_porch
        45,     // vsync_pulse_width
        13,     // vsync_back_porch
        1,      // pclk_active_neg
        15000000 // prefer_speed (15MHz - stable)
    );

    // Create display
    gfx = new Arduino_RGB_Display(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        rgbpanel,
        0,      // rotation
        false   // auto_flush OFF - LVGL controls flushing
    );

    // Initialize
    if (!gfx->begin()) {
        Serial.println("ERROR: Display initialization failed!");
        return;
    }

    // Turn on backlight via CH422G
    ch422g_backlight_on();
    Serial.println("Backlight ON");

    // Clear screen
    gfx->fillScreen(BLACK);

    Serial.printf("Display initialized: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
}

// ============================================================================
// Initialize LVGL
// ============================================================================

void initLVGL() {
    Serial.println("Initializing LVGL...");

    lv_init();

    // Allocate display buffers in PSRAM (larger = smoother updates)
    size_t buf_size = SCREEN_WIDTH * 200;  // 200 lines buffer
    buf1 = (lv_color_t *)ps_malloc(buf_size * sizeof(lv_color_t));
    buf2 = (lv_color_t *)ps_malloc(buf_size * sizeof(lv_color_t));

    if (!buf1 || !buf2) {
        Serial.println("ERROR: Failed to allocate LVGL buffers in PSRAM!");
        return;
    }

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, buf_size);

    // Initialize display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    Serial.println("LVGL initialized");
}

// ============================================================================
// Backlight Control (via CH422G)
// ============================================================================

void setBacklight(uint8_t brightness) {
    // brightness: 0-100 (porcentaje)
    ch422g_set_backlight_pwm(brightness);
}

void backlightOn() {
    ch422g_set_backlight_pwm(80);  // 80% por defecto
}

void backlightOff() {
    ch422g_set_backlight_pwm(1);   // Minimo (no apagar completamente)
}

#endif // DISPLAY_H
