/**
 * keyboard_ui.h - LVGL Virtual Keyboard
 *
 * Full-screen keyboard with glassmorphism style
 */

#ifndef KEYBOARD_UI_H
#define KEYBOARD_UI_H

#include <Arduino.h>
#include <lvgl.h>
#include "config.h"

// ============================================================================
// Keyboard Types
// ============================================================================

enum KeyboardType {
    KB_TYPE_TEXT,       // Normal text (letters, numbers, symbols)
    KB_TYPE_NUMBER,     // Numbers only
    KB_TYPE_PASSWORD,   // Password (shows dots)
    KB_TYPE_COORDS      // Coordinates (numbers, decimal, minus)
};

// ============================================================================
// Keyboard State
// ============================================================================

static lv_obj_t *kb_container = nullptr;
static lv_obj_t *kb_textarea = nullptr;
static lv_obj_t *kb_keyboard = nullptr;
static lv_obj_t *kb_title_label = nullptr;
static lv_obj_t *kb_hint_label = nullptr;
static lv_obj_t *kb_btn_ok = nullptr;
static lv_obj_t *kb_btn_cancel = nullptr;

static bool kb_is_open = false;
static KeyboardType kb_current_type = KB_TYPE_TEXT;

// Callback when keyboard is closed
typedef void (*kb_callback_t)(bool confirmed, const char *text);
static kb_callback_t kb_on_close = nullptr;

// Forward declarations
void closeKeyboard();

// ============================================================================
// Keyboard Styles
// ============================================================================

static lv_style_t style_kb_bg;
static lv_style_t style_kb_panel;
static lv_style_t style_kb_textarea;
static lv_style_t style_kb_btn;
static bool kb_styles_initialized = false;

void initKeyboardStyles() {
    if (kb_styles_initialized) return;

    // Semi-transparent dark background
    lv_style_init(&style_kb_bg);
    lv_style_set_bg_color(&style_kb_bg, lv_color_black());
    lv_style_set_bg_opa(&style_kb_bg, LV_OPA_70);

    // Glass panel
    lv_style_init(&style_kb_panel);
    lv_style_set_bg_color(&style_kb_panel, lv_color_white());
    lv_style_set_bg_opa(&style_kb_panel, LV_OPA_20);
    lv_style_set_radius(&style_kb_panel, 20);
    lv_style_set_border_width(&style_kb_panel, 1);
    lv_style_set_border_color(&style_kb_panel, lv_color_white());
    lv_style_set_border_opa(&style_kb_panel, LV_OPA_30);
    lv_style_set_pad_all(&style_kb_panel, 20);

    // Text area
    lv_style_init(&style_kb_textarea);
    lv_style_set_bg_color(&style_kb_textarea, lv_color_white());
    lv_style_set_bg_opa(&style_kb_textarea, LV_OPA_90);
    lv_style_set_text_color(&style_kb_textarea, lv_color_hex(0x1a1a2e));
    lv_style_set_text_font(&style_kb_textarea, &lv_font_montserrat_24);
    lv_style_set_radius(&style_kb_textarea, 10);
    lv_style_set_border_width(&style_kb_textarea, 2);
    lv_style_set_border_color(&style_kb_textarea, lv_color_hex(0x4ECDC4));
    lv_style_set_pad_all(&style_kb_textarea, 10);

    // Buttons
    lv_style_init(&style_kb_btn);
    lv_style_set_bg_color(&style_kb_btn, lv_color_hex(0x4ECDC4));
    lv_style_set_bg_opa(&style_kb_btn, LV_OPA_COVER);
    lv_style_set_text_color(&style_kb_btn, lv_color_white());
    lv_style_set_radius(&style_kb_btn, 10);
    lv_style_set_pad_all(&style_kb_btn, 10);

    kb_styles_initialized = true;
}

// ============================================================================
// Event Handlers
// ============================================================================

static void kb_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        // Keyboard key pressed - let it update textarea automatically
    }
    else if (code == LV_EVENT_READY) {
        // Enter pressed - confirm
        if (kb_on_close) {
            const char *text = lv_textarea_get_text(kb_textarea);
            kb_on_close(true, text);
        }
        closeKeyboard();
    }
    else if (code == LV_EVENT_CANCEL) {
        // Escape pressed - cancel
        if (kb_on_close) {
            kb_on_close(false, "");
        }
        closeKeyboard();
    }
}

static void kb_ok_handler(lv_event_t *e) {
    if (kb_on_close) {
        const char *text = lv_textarea_get_text(kb_textarea);
        kb_on_close(true, text);
    }
    closeKeyboard();
}

static void kb_cancel_handler(lv_event_t *e) {
    if (kb_on_close) {
        kb_on_close(false, "");
    }
    closeKeyboard();
}

// ============================================================================
// Show Keyboard
// ============================================================================

void showKeyboard(const char *title, const char *hint, const char *initial_value,
                  KeyboardType type, int max_length, kb_callback_t callback) {

    if (kb_is_open) return;

    initKeyboardStyles();
    kb_on_close = callback;
    kb_current_type = type;

    // Create fullscreen container
    kb_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(kb_container, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_pos(kb_container, 0, 0);
    lv_obj_add_style(kb_container, &style_kb_bg, 0);
    lv_obj_clear_flag(kb_container, LV_OBJ_FLAG_SCROLLABLE);

    // Create panel
    lv_obj_t *panel = lv_obj_create(kb_container);
    lv_obj_set_size(panel, 900, 550);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(panel, &style_kb_panel, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    // Title
    kb_title_label = lv_label_create(panel);
    lv_label_set_text(kb_title_label, title);
    lv_obj_set_style_text_font(kb_title_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(kb_title_label, lv_color_white(), 0);
    lv_obj_align(kb_title_label, LV_ALIGN_TOP_LEFT, 10, 5);

    // Hint
    if (hint && strlen(hint) > 0) {
        kb_hint_label = lv_label_create(panel);
        lv_label_set_text(kb_hint_label, hint);
        lv_obj_set_style_text_font(kb_hint_label, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(kb_hint_label, lv_color_hex(0xcccccc), 0);
        lv_obj_align(kb_hint_label, LV_ALIGN_TOP_LEFT, 10, 35);
    }

    // Text area
    kb_textarea = lv_textarea_create(panel);
    lv_obj_set_size(kb_textarea, 760, 50);
    lv_obj_align(kb_textarea, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_add_style(kb_textarea, &style_kb_textarea, 0);
    lv_textarea_set_one_line(kb_textarea, true);
    lv_textarea_set_max_length(kb_textarea, max_length);

    if (type == KB_TYPE_PASSWORD) {
        lv_textarea_set_password_mode(kb_textarea, true);
    }

    if (initial_value && strlen(initial_value) > 0) {
        lv_textarea_set_text(kb_textarea, initial_value);
    }

    // Keyboard
    kb_keyboard = lv_keyboard_create(panel);
    lv_obj_set_size(kb_keyboard, 860, 280);
    lv_obj_align(kb_keyboard, LV_ALIGN_BOTTOM_MID, 0, -60);
    lv_keyboard_set_textarea(kb_keyboard, kb_textarea);

    // Set keyboard mode based on type
    if (type == KB_TYPE_NUMBER || type == KB_TYPE_COORDS) {
        lv_keyboard_set_mode(kb_keyboard, LV_KEYBOARD_MODE_NUMBER);
    } else {
        lv_keyboard_set_mode(kb_keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
    }

    lv_obj_add_event_cb(kb_keyboard, kb_event_handler, LV_EVENT_ALL, NULL);

    // OK button
    kb_btn_ok = lv_btn_create(panel);
    lv_obj_set_size(kb_btn_ok, 120, 45);
    lv_obj_align(kb_btn_ok, LV_ALIGN_BOTTOM_RIGHT, -10, -5);
    lv_obj_add_style(kb_btn_ok, &style_kb_btn, 0);
    lv_obj_add_event_cb(kb_btn_ok, kb_ok_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *ok_label = lv_label_create(kb_btn_ok);
    lv_label_set_text(ok_label, "Aceptar");
    lv_obj_center(ok_label);

    // Cancel button
    kb_btn_cancel = lv_btn_create(panel);
    lv_obj_set_size(kb_btn_cancel, 120, 45);
    lv_obj_align(kb_btn_cancel, LV_ALIGN_BOTTOM_RIGHT, -140, -5);
    lv_obj_set_style_bg_color(kb_btn_cancel, lv_color_hex(0x666666), 0);
    lv_obj_add_event_cb(kb_btn_cancel, kb_cancel_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *cancel_label = lv_label_create(kb_btn_cancel);
    lv_label_set_text(cancel_label, "Cancelar");
    lv_obj_center(cancel_label);

    kb_is_open = true;

    // Animation: fade in
    lv_obj_set_style_opa(kb_container, LV_OPA_TRANSP, 0);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, kb_container);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_time(&a, 200);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    lv_anim_start(&a);
}

// ============================================================================
// Close Keyboard
// ============================================================================

void closeKeyboard() {
    if (!kb_is_open || !kb_container) return;

    // Animation: fade out then delete
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, kb_container);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_time(&a, 150);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    lv_anim_set_ready_cb(&a, [](lv_anim_t *a) {
        lv_obj_t *obj = (lv_obj_t *)a->var;
        lv_obj_del(obj);
    });
    lv_anim_start(&a);

    kb_container = nullptr;
    kb_textarea = nullptr;
    kb_keyboard = nullptr;
    kb_title_label = nullptr;
    kb_hint_label = nullptr;
    kb_btn_ok = nullptr;
    kb_btn_cancel = nullptr;
    kb_on_close = nullptr;
    kb_is_open = false;
}

// ============================================================================
// Quick Input Helpers
// ============================================================================

void showTextInput(const char *title, const char *initial, kb_callback_t callback) {
    showKeyboard(title, "", initial, KB_TYPE_TEXT, 64, callback);
}

void showPasswordInput(const char *title, kb_callback_t callback) {
    showKeyboard(title, "La contrasena se oculta", "", KB_TYPE_PASSWORD, 64, callback);
}

void showNumberInput(const char *title, const char *initial, kb_callback_t callback) {
    showKeyboard(title, "", initial, KB_TYPE_NUMBER, 16, callback);
}

void showCoordInput(const char *title, const char *hint, const char *initial, kb_callback_t callback) {
    showKeyboard(title, hint, initial, KB_TYPE_COORDS, 16, callback);
}

bool isKeyboardOpen() {
    return kb_is_open;
}

#endif // KEYBOARD_UI_H
