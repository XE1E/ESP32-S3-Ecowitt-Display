/**
 * touch.h - GT911 Touch Controller using GT911 Lite library
 *
 * For Waveshare ESP32-S3-Touch-LCD-7B
 */

#ifndef TOUCH_H
#define TOUCH_H

#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include <gt911_lite.h>
#include "config.h"

// CH422G functions from display.h
extern void ch422g_set_pin(uint8_t pin, bool state);
#define CH422G_TOUCH_RST_PIN 1

// GT911 instance
GT911_Lite ts;

// Touch data
static int16_t touch_x = 0;
static int16_t touch_y = 0;
static bool touch_pressed = false;

// LVGL input device
static lv_indev_drv_t indev_drv;

// ============================================================================
// LVGL Touch Callback
// ============================================================================

void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    if (ts.read()) {
        if (ts.isTouched) {
            touch_x = ts.x;
            touch_y = ts.y;
            touch_pressed = true;
            data->state = LV_INDEV_STATE_PRESSED;
            data->point.x = touch_x;
            data->point.y = touch_y;
        } else {
            touch_pressed = false;
            data->state = LV_INDEV_STATE_RELEASED;
        }
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// ============================================================================
// Initialize Touch
// ============================================================================

void initTouch() {
    Serial.println("[TOUCH] Initializing GT911...");

    // Reset sequence via CH422G
    pinMode(TOUCH_INT, OUTPUT);
    digitalWrite(TOUCH_INT, LOW);  // Select address 0x5D

    ch422g_set_pin(CH422G_TOUCH_RST_PIN, false);  // RST LOW
    delay(100);
    ch422g_set_pin(CH422G_TOUCH_RST_PIN, true);   // RST HIGH
    delay(200);

    pinMode(TOUCH_INT, INPUT);
    delay(50);

    // Initialize GT911 library
    ts.begin(&Wire);

    Serial.println("[TOUCH] GT911 initialized");

    // Register LVGL input device
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_touch_cb;
    lv_indev_drv_register(&indev_drv);

    Serial.println("[TOUCH] LVGL input registered");
}

// ============================================================================
// Handle Touch (called from loop - not needed with LVGL callbacks)
// ============================================================================

void handleTouch() {
    // Touch handling is done via LVGL callbacks
}

// ============================================================================
// Get Last Touch Position
// ============================================================================

void getLastTouch(int16_t *x, int16_t *y, bool *pressed) {
    *x = touch_x;
    *y = touch_y;
    *pressed = touch_pressed;
}

#endif // TOUCH_H
