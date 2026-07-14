/**
 * backgrounds.h - Background Image Management
 *
 * Loads and manages background images from SD card or LittleFS
 */

#ifndef BACKGROUNDS_H
#define BACKGROUNDS_H

#include <Arduino.h>
#include <lvgl.h>
#include "config.h"

#ifdef USE_SD_CARD
#include <SD.h>
#endif

#include <LittleFS.h>

// ============================================================================
// Background Configuration
// ============================================================================

#define MAX_BACKGROUNDS 10
#define BG_PATH_SD "/backgrounds"
#define BG_PATH_FS "/bg"

struct BackgroundInfo {
    char filename[32];
    char name[32];
    bool on_sd;  // true = SD card, false = LittleFS
};

static BackgroundInfo backgrounds[MAX_BACKGROUNDS];
static int backgroundCount = 0;
static int currentBackgroundIndex = 0;
static lv_obj_t *bg_image_obj = nullptr;
static lv_img_dsc_t bg_img_dsc;
static uint8_t *bg_buffer = nullptr;

// Forward declarations
bool loadRawBackground(const char *path, bool from_sd, lv_obj_t *target);

// ============================================================================
// Initialize Background System
// ============================================================================

bool initBackgrounds() {
    Serial.println("Initializing backgrounds...");
    backgroundCount = 0;

    // Initialize LittleFS
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
    } else {
        // Scan LittleFS for backgrounds
        File root = LittleFS.open(BG_PATH_FS);
        if (root && root.isDirectory()) {
            File file = root.openNextFile();
            while (file && backgroundCount < MAX_BACKGROUNDS) {
                String name = file.name();
                if (name.endsWith(".jpg") || name.endsWith(".png") || name.endsWith(".bin")) {
                    strlcpy(backgrounds[backgroundCount].filename, file.name(), 32);
                    // Extract name without extension
                    String shortName = name.substring(0, name.lastIndexOf('.'));
                    strlcpy(backgrounds[backgroundCount].name, shortName.c_str(), 32);
                    backgrounds[backgroundCount].on_sd = false;
                    backgroundCount++;
                    Serial.printf("Found LittleFS bg: %s\n", file.name());
                }
                file = root.openNextFile();
            }
        }
    }

    #ifdef USE_SD_CARD
    // Scan SD card for backgrounds
    if (SD.exists(BG_PATH_SD)) {
        File root = SD.open(BG_PATH_SD);
        if (root && root.isDirectory()) {
            File file = root.openNextFile();
            while (file && backgroundCount < MAX_BACKGROUNDS) {
                String name = file.name();
                if (name.endsWith(".jpg") || name.endsWith(".png") || name.endsWith(".bin")) {
                    strlcpy(backgrounds[backgroundCount].filename, file.name(), 32);
                    String shortName = name.substring(0, name.lastIndexOf('.'));
                    strlcpy(backgrounds[backgroundCount].name, shortName.c_str(), 32);
                    backgrounds[backgroundCount].on_sd = true;
                    backgroundCount++;
                    Serial.printf("Found SD bg: %s\n", file.name());
                }
                file = root.openNextFile();
            }
        }
    }
    #endif

    Serial.printf("Total backgrounds found: %d\n", backgroundCount);
    return backgroundCount > 0;
}

// ============================================================================
// Get Background Count and Names
// ============================================================================

int getBackgroundCount() {
    return backgroundCount;
}

const char* getBackgroundName(int index) {
    if (index < 0 || index >= backgroundCount) return "";
    return backgrounds[index].name;
}

// ============================================================================
// Load Background Image
// ============================================================================

bool loadBackground(int index, lv_obj_t *target) {
    if (index < 0 || index >= backgroundCount) {
        Serial.println("Invalid background index");
        return false;
    }

    BackgroundInfo *bg = &backgrounds[index];
    char path[64];

    if (bg->on_sd) {
        snprintf(path, sizeof(path), "%s/%s", BG_PATH_SD, bg->filename);
    } else {
        snprintf(path, sizeof(path), "%s/%s", BG_PATH_FS, bg->filename);
    }

    Serial.printf("Loading background: %s\n", path);

    // Check if it's a .bin file (raw RGB565)
    String filename = bg->filename;
    if (filename.endsWith(".bin")) {
        return loadRawBackground(path, bg->on_sd, target);
    }

    // For JPG/PNG, use LVGL's built-in decoder (requires SD/FS driver)
    // This is a simplified version - full implementation needs LVGL FS driver
    Serial.println("JPG/PNG loading requires LVGL FS driver setup");
    return false;
}

// ============================================================================
// Load Raw RGB565 Background
// ============================================================================

bool loadRawBackground(const char *path, bool from_sd, lv_obj_t *target) {
    File file;

    #ifdef USE_SD_CARD
    if (from_sd) {
        file = SD.open(path, FILE_READ);
    } else {
        file = LittleFS.open(path, FILE_READ);
    }
    #else
    file = LittleFS.open(path, FILE_READ);
    #endif

    if (!file) {
        Serial.printf("Failed to open: %s\n", path);
        return false;
    }

    size_t fileSize = file.size();
    size_t expectedSize = SCREEN_WIDTH * SCREEN_HEIGHT * 2;  // RGB565 = 2 bytes/pixel

    if (fileSize != expectedSize) {
        Serial.printf("Size mismatch: %d vs expected %d\n", fileSize, expectedSize);
        file.close();
        return false;
    }

    // Allocate buffer in PSRAM
    if (bg_buffer) {
        free(bg_buffer);
    }
    bg_buffer = (uint8_t *)ps_malloc(fileSize);

    if (!bg_buffer) {
        Serial.println("Failed to allocate background buffer");
        file.close();
        return false;
    }

    // Read file
    size_t bytesRead = file.read(bg_buffer, fileSize);
    file.close();

    if (bytesRead != fileSize) {
        Serial.println("Failed to read complete file");
        free(bg_buffer);
        bg_buffer = nullptr;
        return false;
    }

    // Create LVGL image descriptor
    bg_img_dsc.header.always_zero = 0;
    bg_img_dsc.header.w = SCREEN_WIDTH;
    bg_img_dsc.header.h = SCREEN_HEIGHT;
    bg_img_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    bg_img_dsc.data_size = fileSize;
    bg_img_dsc.data = bg_buffer;

    // Create or update image object
    if (!bg_image_obj) {
        bg_image_obj = lv_img_create(target);
        lv_obj_set_pos(bg_image_obj, 0, 0);
        lv_obj_move_to_index(bg_image_obj, 0);  // Send to back
    }

    lv_img_set_src(bg_image_obj, &bg_img_dsc);

    currentBackgroundIndex = 0;
    for (int i = 0; i < backgroundCount; i++) {
        if (strcmp(backgrounds[i].filename, path) == 0) {
            currentBackgroundIndex = i;
            break;
        }
    }

    Serial.println("Background loaded successfully");
    return true;
}

// ============================================================================
// Set Gradient Background (Fallback)
// ============================================================================

void setGradientBackground(lv_obj_t *target, uint32_t color1, uint32_t color2) {
    lv_obj_set_style_bg_color(target, lv_color_hex(color1), 0);
    lv_obj_set_style_bg_grad_color(target, lv_color_hex(color2), 0);
    lv_obj_set_style_bg_grad_dir(target, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(target, LV_OPA_COVER, 0);
}

// ============================================================================
// Predefined Gradient Themes
// ============================================================================

struct GradientTheme {
    const char *name;
    uint32_t color1;
    uint32_t color2;
};

const GradientTheme gradientThemes[] = {
    {"Amanecer",    0xFF7E5F, 0xFEB47B},  // Warm orange-pink
    {"Atardecer",   0x834d9b, 0xd04ed6},  // Purple sunset
    {"Noche",       0x0f0c29, 0x302b63},  // Dark blue
    {"Cielo",       0x56CCF2, 0x2F80ED},  // Sky blue
    {"Bosque",      0x134E5E, 0x71B280},  // Green forest
    {"Oceano",      0x1A2980, 0x26D0CE},  // Ocean blue-teal
    {"Tormenta",    0x373B44, 0x4286f4},  // Storm gray-blue
    {"Aurora",      0x00C9FF, 0x92FE9D},  // Northern lights
};
const int gradientThemeCount = sizeof(gradientThemes) / sizeof(gradientThemes[0]);

void setThemeBackground(lv_obj_t *target, int themeIndex) {
    if (themeIndex < 0 || themeIndex >= gradientThemeCount) {
        themeIndex = 0;
    }
    setGradientBackground(target, gradientThemes[themeIndex].color1,
                          gradientThemes[themeIndex].color2);
}

const char* getThemeName(int index) {
    if (index < 0 || index >= gradientThemeCount) return "";
    return gradientThemes[index].name;
}

int getThemeCount() {
    return gradientThemeCount;
}

// ============================================================================
// Auto Background Based on Time
// ============================================================================

void setAutoBackground(lv_obj_t *target) {
    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);
    int hour = timeinfo.tm_hour;

    int themeIndex;
    if (hour >= 5 && hour < 8) {
        themeIndex = 0;  // Amanecer
    } else if (hour >= 8 && hour < 17) {
        themeIndex = 3;  // Cielo
    } else if (hour >= 17 && hour < 20) {
        themeIndex = 1;  // Atardecer
    } else {
        themeIndex = 2;  // Noche
    }

    setThemeBackground(target, themeIndex);
}

#endif // BACKGROUNDS_H
