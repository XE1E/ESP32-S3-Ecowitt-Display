/**
 * setup_wizard.h - First Boot Configuration Wizard
 *
 * Screen-based architecture - each step is a separate screen
 * Multi-language support (ES/EN)
 */

#ifndef SETUP_WIZARD_H
#define SETUP_WIZARD_H

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include "config.h"
#include "config_data.h"
#include "localization.h"

// ============================================================================
// Wizard State
// ============================================================================

enum WizardStep {
    WIZARD_LANGUAGE = 0,
    WIZARD_WELCOME,
    WIZARD_WIFI_SCANNING,  // Shows "Scanning..." then transitions to WIFI
    WIZARD_WIFI,
    WIZARD_WIFI_PASSWORD,
    WIZARD_WIFI_ADD_MORE,
    WIZARD_API_KEY,
    WIZARD_LOCATION,
    WIZARD_DISPLAY,
    WIZARD_COMPLETE
};

static WizardStep currentWizardStep = WIZARD_LANGUAGE;
static lv_obj_t *currentScreen = nullptr;

// WiFi scan results
#define MAX_WIFI_NETWORKS 15
static String wifiNetworks[MAX_WIFI_NETWORKS];
static int wifiRSSI[MAX_WIFI_NETWORKS];
static int wifiCount = 0;
static int selectedWifiIndex = -1;
static String selectedSSID = "";
static int currentWifiSlot = 0;
static bool wifiScanPending = false;  // Flag for deferred WiFi scan

// Location search results
#define MAX_LOCATION_RESULTS 10
static String locationNames[MAX_LOCATION_RESULTS];
static String locationRegions[MAX_LOCATION_RESULTS];
static float locationLats[MAX_LOCATION_RESULTS];
static float locationLons[MAX_LOCATION_RESULTS];
static int locationCount = 0;

// Styles
static lv_style_t style_wizard_bg;
static lv_style_t style_wizard_btn;
static lv_style_t style_wizard_btn_primary;
static lv_style_t style_wizard_btn_secondary;
static lv_style_t style_wizard_card;
static lv_style_t style_wizard_list_item;
static bool wizard_styles_init = false;

// ============================================================================
// Forward Declarations
// ============================================================================

void showWizardStep(WizardStep step);
lv_obj_t* createWizardScreen(const char* title, const char* subtitle, int stepNum, int totalSteps);
void updateLoopCounter();
void processWizardTransition();
void initialWiFiScan();
void showWiFiScanningStep();
bool connectToConfiguredWiFi();
bool searchLocations(const char* query);

// ============================================================================
// Initialize Styles
// ============================================================================

void initWizardStyles() {
    if (wizard_styles_init) return;

    lv_style_init(&style_wizard_bg);
    lv_style_set_bg_color(&style_wizard_bg, lv_color_hex(0x1a1a2e));
    lv_style_set_bg_grad_color(&style_wizard_bg, lv_color_hex(0x16213e));
    lv_style_set_bg_grad_dir(&style_wizard_bg, LV_GRAD_DIR_VER);

    lv_style_init(&style_wizard_btn);
    lv_style_set_bg_color(&style_wizard_btn, lv_color_white());
    lv_style_set_bg_opa(&style_wizard_btn, LV_OPA_20);
    lv_style_set_text_color(&style_wizard_btn, lv_color_white());
    lv_style_set_radius(&style_wizard_btn, 12);
    lv_style_set_pad_all(&style_wizard_btn, 12);
    lv_style_set_border_width(&style_wizard_btn, 0);

    lv_style_init(&style_wizard_btn_primary);
    lv_style_set_bg_color(&style_wizard_btn_primary, lv_color_hex(0x4ECDC4));
    lv_style_set_bg_opa(&style_wizard_btn_primary, LV_OPA_COVER);
    lv_style_set_text_color(&style_wizard_btn_primary, lv_color_white());
    lv_style_set_radius(&style_wizard_btn_primary, 12);
    lv_style_set_pad_all(&style_wizard_btn_primary, 12);
    lv_style_set_border_width(&style_wizard_btn_primary, 0);

    lv_style_init(&style_wizard_btn_secondary);
    lv_style_set_bg_color(&style_wizard_btn_secondary, lv_color_hex(0x555555));
    lv_style_set_bg_opa(&style_wizard_btn_secondary, LV_OPA_COVER);
    lv_style_set_text_color(&style_wizard_btn_secondary, lv_color_white());
    lv_style_set_radius(&style_wizard_btn_secondary, 12);
    lv_style_set_pad_all(&style_wizard_btn_secondary, 12);
    lv_style_set_border_width(&style_wizard_btn_secondary, 0);

    lv_style_init(&style_wizard_card);
    lv_style_set_bg_color(&style_wizard_card, lv_color_white());
    lv_style_set_bg_opa(&style_wizard_card, LV_OPA_10);
    lv_style_set_radius(&style_wizard_card, 16);
    lv_style_set_pad_all(&style_wizard_card, 15);
    lv_style_set_border_width(&style_wizard_card, 1);
    lv_style_set_border_color(&style_wizard_card, lv_color_white());
    lv_style_set_border_opa(&style_wizard_card, LV_OPA_20);

    lv_style_init(&style_wizard_list_item);
    lv_style_set_bg_color(&style_wizard_list_item, lv_color_white());
    lv_style_set_bg_opa(&style_wizard_list_item, LV_OPA_10);
    lv_style_set_radius(&style_wizard_list_item, 8);
    lv_style_set_pad_all(&style_wizard_list_item, 10);
    lv_style_set_text_color(&style_wizard_list_item, lv_color_white());

    wizard_styles_init = true;
}

// ============================================================================
// Create Base Screen
// ============================================================================

lv_obj_t* createWizardScreen(const char* title, const char* subtitle, int stepNum, int totalSteps) {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, &style_wizard_bg, 0);

    if (stepNum > 0 && totalSteps > 0) {
        lv_obj_t *progress = lv_bar_create(scr);
        lv_obj_set_size(progress, SCREEN_WIDTH - 100, 6);
        lv_obj_align(progress, LV_ALIGN_TOP_MID, 0, 15);
        lv_bar_set_range(progress, 0, 100);
        lv_bar_set_value(progress, (stepNum * 100) / totalSteps, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(progress, lv_color_hex(0x333333), LV_PART_MAIN);
        lv_obj_set_style_bg_color(progress, lv_color_hex(0x4ECDC4), LV_PART_INDICATOR);
        lv_obj_set_style_radius(progress, 3, 0);
    }

    if (title && strlen(title) > 0) {
        lv_obj_t *lbl_title = lv_label_create(scr);
        lv_label_set_text(lbl_title, title);
        lv_obj_set_style_text_color(lbl_title, lv_color_white(), 0);
        lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_28, 0);
        lv_obj_align(lbl_title, LV_ALIGN_TOP_LEFT, 50, 35);
    }

    if (subtitle && strlen(subtitle) > 0) {
        lv_obj_t *lbl_sub = lv_label_create(scr);
        lv_label_set_text(lbl_sub, subtitle);
        lv_obj_set_style_text_color(lbl_sub, lv_color_hex(0x888888), 0);
        lv_obj_set_style_text_font(lbl_sub, &lv_font_montserrat_16, 0);
        lv_obj_align(lbl_sub, LV_ALIGN_TOP_LEFT, 50, 70);
    }

    return scr;
}

// ============================================================================
// WiFi Functions
// ============================================================================

void initialWiFiScan() {
    Serial.println("Scanning WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    wifiCount = min(n, MAX_WIFI_NETWORKS);
    for (int i = 0; i < wifiCount; i++) {
        wifiNetworks[i] = WiFi.SSID(i);
        wifiRSSI[i] = WiFi.RSSI(i);
    }
    Serial.printf("Found %d networks\n", wifiCount);
    WiFi.scanDelete();
}

bool connectToConfiguredWiFi() {
    if (!appConfig.wifi[0].enabled || strlen(appConfig.wifi[0].ssid) == 0) return false;
    Serial.printf("Connecting to %s...\n", appConfig.wifi[0].ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(appConfig.wifi[0].ssid, appConfig.wifi[0].password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
    }
    return WiFi.status() == WL_CONNECTED;
}

// ============================================================================
// Location Search
// ============================================================================

bool searchLocations(const char* query) {
    if (strlen(appConfig.api.weather_api_key) == 0 || WiFi.status() != WL_CONNECTED) return false;

    String url = "https://api.weatherapi.com/v1/search.json?key=";
    url += appConfig.api.weather_api_key;
    url += "&q=";
    url += query;

    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode != 200) { http.end(); return false; }

    String payload = http.getString();
    http.end();

    DynamicJsonDocument doc(4096);
    if (deserializeJson(doc, payload)) return false;

    JsonArray results = doc.as<JsonArray>();
    locationCount = min((int)results.size(), MAX_LOCATION_RESULTS);

    for (int i = 0; i < locationCount; i++) {
        locationNames[i] = results[i]["name"].as<String>();
        locationRegions[i] = results[i]["region"].as<String>() + ", " + results[i]["country"].as<String>();
        locationLats[i] = results[i]["lat"].as<float>();
        locationLons[i] = results[i]["lon"].as<float>();
    }
    return locationCount > 0;
}

// ============================================================================
// Step 0: Language Selection
// ============================================================================

void showLanguageStep() {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, &style_wizard_bg, 0);

    // Globe icon
    lv_obj_t *icon = lv_label_create(scr);
    lv_label_set_text(icon, LV_SYMBOL_SETTINGS);  // Using settings as globe alternative
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x4ECDC4), 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -120);

    // Title (shown in both languages)
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Select Language / Selecciona Idioma");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -50);

    // Spanish button with flag colors
    lv_obj_t *btn_es = lv_btn_create(scr);
    lv_obj_set_size(btn_es, 280, 80);
    lv_obj_align(btn_es, LV_ALIGN_CENTER, -160, 40);
    lv_obj_set_style_bg_color(btn_es, lv_color_hex(0xC60B1E), 0);  // Spanish red
    lv_obj_set_style_radius(btn_es, 16, 0);
    lv_obj_add_event_cb(btn_es, [](lv_event_t *e) {
        setLanguage(LANG_ES);
        showWizardStep(WIZARD_WELCOME);
    }, LV_EVENT_CLICKED, NULL);

    // Spanish flag stripe (yellow)
    lv_obj_t *stripe_es = lv_obj_create(btn_es);
    lv_obj_set_size(stripe_es, 280, 25);
    lv_obj_align(stripe_es, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(stripe_es, lv_color_hex(0xFFC400), 0);
    lv_obj_set_style_border_width(stripe_es, 0, 0);
    lv_obj_set_style_radius(stripe_es, 0, 0);
    lv_obj_clear_flag(stripe_es, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *lbl_es = lv_label_create(btn_es);
    lv_label_set_text(lbl_es, "Espanol");
    lv_obj_set_style_text_font(lbl_es, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_es, lv_color_white(), 0);
    lv_obj_align(lbl_es, LV_ALIGN_BOTTOM_MID, 0, -8);

    // English button with flag colors
    lv_obj_t *btn_en = lv_btn_create(scr);
    lv_obj_set_size(btn_en, 280, 80);
    lv_obj_align(btn_en, LV_ALIGN_CENTER, 160, 40);
    lv_obj_set_style_bg_color(btn_en, lv_color_hex(0x012169), 0);  // UK blue
    lv_obj_set_style_radius(btn_en, 16, 0);
    lv_obj_add_event_cb(btn_en, [](lv_event_t *e) {
        setLanguage(LANG_EN);
        showWizardStep(WIZARD_WELCOME);
    }, LV_EVENT_CLICKED, NULL);

    // UK cross stripes (simplified)
    lv_obj_t *stripe_en_h = lv_obj_create(btn_en);
    lv_obj_set_size(stripe_en_h, 280, 15);
    lv_obj_align(stripe_en_h, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(stripe_en_h, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(stripe_en_h, 0, 0);
    lv_obj_set_style_radius(stripe_en_h, 0, 0);
    lv_obj_clear_flag(stripe_en_h, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *stripe_en_v = lv_obj_create(btn_en);
    lv_obj_set_size(stripe_en_v, 15, 80);
    lv_obj_align(stripe_en_v, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(stripe_en_v, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(stripe_en_v, 0, 0);
    lv_obj_set_style_radius(stripe_en_v, 0, 0);
    lv_obj_clear_flag(stripe_en_v, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *lbl_en = lv_label_create(btn_en);
    lv_label_set_text(lbl_en, "English");
    lv_obj_set_style_text_font(lbl_en, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_en, lv_color_white(), 0);
    lv_obj_align(lbl_en, LV_ALIGN_BOTTOM_MID, 0, -8);

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Step 1: Welcome
// ============================================================================

void showWelcomeStep() {
    lv_obj_t *scr = createWizardScreen("", "", 0, 6);

    lv_obj_t *icon = lv_label_create(scr);
    lv_label_set_text(icon, LV_SYMBOL_HOME);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x4ECDC4), 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -100);

    lv_obj_t *name = lv_label_create(scr);
    lv_label_set_text(name, tr(STR_APP_NAME));
    lv_obj_set_style_text_font(name, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(name, lv_color_white(), 0);
    lv_obj_align(name, LV_ALIGN_CENTER, 0, -40);

    lv_obj_t *desc = lv_label_create(scr);
    lv_label_set_text(desc, tr(STR_WELCOME_DESC));
    lv_obj_set_style_text_color(desc, lv_color_hex(0xaaaaaa), 0);
    lv_obj_set_style_text_font(desc, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_align(desc, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(desc, LV_ALIGN_CENTER, 0, 20);

    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_add_style(btn, &style_wizard_btn_primary, 0);
    lv_obj_set_size(btn, 250, 55);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 120);
    lv_obj_add_event_cb(btn, [](lv_event_t *e) {
        currentWifiSlot = 0;
        wifiCount = 0;
        showWizardStep(WIZARD_WIFI_SCANNING);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn);
    char btn_text[32];
    snprintf(btn_text, sizeof(btn_text), "%s " LV_SYMBOL_RIGHT, tr(STR_BEGIN));
    lv_label_set_text(btn_label, btn_text);
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_18, 0);
    lv_obj_center(btn_label);

    // Change language button (small, bottom left)
    lv_obj_t *btn_lang = lv_btn_create(scr);
    lv_obj_add_style(btn_lang, &style_wizard_btn, 0);
    lv_obj_set_size(btn_lang, 45, 45);
    lv_obj_align(btn_lang, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_add_event_cb(btn_lang, [](lv_event_t *e) {
        showWizardStep(WIZARD_LANGUAGE);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lang_icon = lv_label_create(btn_lang);
    lv_label_set_text(lang_icon, LV_SYMBOL_SETTINGS);
    lv_obj_center(lang_icon);

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Step 2a: WiFi Scanning (shows message, sets flag for main loop)
// ============================================================================

void showWiFiScanningStep() {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_add_style(scr, &style_wizard_bg, 0);

    // Scanning icon
    lv_obj_t *icon = lv_label_create(scr);
    lv_label_set_text(icon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x4ECDC4), 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -50);

    // Scanning message
    lv_obj_t *msg = lv_label_create(scr);
    lv_label_set_text(msg, tr(STR_WIFI_SCANNING));
    lv_obj_set_style_text_font(msg, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(msg, lv_color_white(), 0);
    lv_obj_align(msg, LV_ALIGN_CENTER, 0, 30);

    lv_scr_load(scr);
    currentScreen = scr;

    // Set flag - main loop will do the scan and transition
    wifiScanPending = true;
}

// ============================================================================
// Step 2b: WiFi Selection (shows results)
// ============================================================================

void showWiFiStep() {
    char title[32];
    snprintf(title, sizeof(title), "%s %d", tr(STR_WIFI_TITLE), currentWifiSlot + 1);
    lv_obj_t *scr = createWizardScreen(title, tr(STR_WIFI_SELECT), 1, 6);

    // Don't scan here - scan is done before entering this step

    lv_obj_t *list_cont = lv_obj_create(scr);
    lv_obj_set_size(list_cont, SCREEN_WIDTH - 100, 380);
    lv_obj_align(list_cont, LV_ALIGN_TOP_MID, 0, 90);
    lv_obj_add_style(list_cont, &style_wizard_card, 0);
    lv_obj_set_flex_flow(list_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(list_cont, 8, 0);
    lv_obj_set_scrollbar_mode(list_cont, LV_SCROLLBAR_MODE_AUTO);

    if (wifiCount == 0) {
        lv_obj_t *no_wifi = lv_label_create(list_cont);
        lv_label_set_text(no_wifi, tr(STR_WIFI_NO_NETWORKS));
        lv_obj_set_style_text_color(no_wifi, lv_color_hex(0xff6666), 0);
    } else {
        for (int i = 0; i < wifiCount; i++) {
            lv_obj_t *item = lv_btn_create(list_cont);
            lv_obj_add_style(item, &style_wizard_list_item, 0);
            lv_obj_set_size(item, SCREEN_WIDTH - 140, 50);
            lv_obj_set_user_data(item, (void*)(intptr_t)i);
            lv_obj_add_event_cb(item, [](lv_event_t *e) {
                int idx = (int)(intptr_t)lv_obj_get_user_data(lv_event_get_target(e));
                selectedWifiIndex = idx;
                selectedSSID = wifiNetworks[idx];
                strlcpy(appConfig.wifi[currentWifiSlot].ssid, wifiNetworks[idx].c_str(),
                        sizeof(appConfig.wifi[currentWifiSlot].ssid));
                showWizardStep(WIZARD_WIFI_PASSWORD);
            }, LV_EVENT_CLICKED, NULL);

            lv_obj_t *icon = lv_label_create(item);
            lv_label_set_text(icon, LV_SYMBOL_WIFI);
            lv_obj_set_style_text_color(icon, lv_color_hex(0x4ECDC4), 0);
            lv_obj_align(icon, LV_ALIGN_LEFT_MID, 5, 0);

            lv_obj_t *ssid = lv_label_create(item);
            lv_label_set_text(ssid, wifiNetworks[i].c_str());
            lv_obj_align(ssid, LV_ALIGN_LEFT_MID, 40, 0);

            char rssi_str[16];
            snprintf(rssi_str, sizeof(rssi_str), "%d dBm", wifiRSSI[i]);
            lv_obj_t *rssi = lv_label_create(item);
            lv_label_set_text(rssi, rssi_str);
            lv_obj_set_style_text_color(rssi, lv_color_hex(0x888888), 0);
            lv_obj_set_style_text_font(rssi, &lv_font_montserrat_14, 0);
            lv_obj_align(rssi, LV_ALIGN_RIGHT_MID, -10, 0);
        }
    }

    // Scan button
    lv_obj_t *btn_scan = lv_btn_create(scr);
    lv_obj_add_style(btn_scan, &style_wizard_btn_secondary, 0);
    lv_obj_set_size(btn_scan, 150, 45);
    lv_obj_align(btn_scan, LV_ALIGN_BOTTOM_LEFT, 50, -25);
    lv_obj_add_event_cb(btn_scan, [](lv_event_t *e) {
        wifiCount = 0;
        showWizardStep(WIZARD_WIFI_SCANNING);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *scan_label = lv_label_create(btn_scan);
    char scan_text[32];
    snprintf(scan_text, sizeof(scan_text), LV_SYMBOL_REFRESH " %s", tr(STR_SCAN));
    lv_label_set_text(scan_label, scan_text);
    lv_obj_center(scan_label);

    // Skip button (only for first WiFi)
    if (currentWifiSlot == 0) {
        lv_obj_t *btn_skip = lv_btn_create(scr);
        lv_obj_add_style(btn_skip, &style_wizard_btn, 0);
        lv_obj_set_size(btn_skip, 120, 45);
        lv_obj_align(btn_skip, LV_ALIGN_BOTTOM_RIGHT, -50, -25);
        lv_obj_add_event_cb(btn_skip, [](lv_event_t *e) {
            showWizardStep(WIZARD_API_KEY);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *skip_label = lv_label_create(btn_skip);
        lv_label_set_text(skip_label, tr(STR_SKIP));
        lv_obj_center(skip_label);
    }

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Step 3: WiFi Password
// ============================================================================

static lv_obj_t *pwd_textarea = nullptr;

void showWiFiPasswordStep() {
    char title[64];
    snprintf(title, sizeof(title), "%s: %s", tr(STR_WIFI_PASSWORD), selectedSSID.c_str());
    lv_obj_t *scr = createWizardScreen(title, "", 1, 6);

    pwd_textarea = lv_textarea_create(scr);
    lv_obj_set_size(pwd_textarea, SCREEN_WIDTH - 250, 50);
    lv_obj_align(pwd_textarea, LV_ALIGN_TOP_MID, -25, 95);
    lv_textarea_set_placeholder_text(pwd_textarea, tr(STR_WIFI_PASSWORD));
    lv_textarea_set_password_mode(pwd_textarea, true);
    lv_textarea_set_one_line(pwd_textarea, true);
    lv_obj_set_style_text_font(pwd_textarea, &lv_font_montserrat_20, 0);

    lv_obj_t *btn_eye = lv_btn_create(scr);
    lv_obj_add_style(btn_eye, &style_wizard_btn, 0);
    lv_obj_set_size(btn_eye, 50, 50);
    lv_obj_align_to(btn_eye, pwd_textarea, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_add_event_cb(btn_eye, [](lv_event_t *e) {
        bool is_pwd = lv_textarea_get_password_mode(pwd_textarea);
        lv_textarea_set_password_mode(pwd_textarea, !is_pwd);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *eye_icon = lv_label_create(btn_eye);
    lv_label_set_text(eye_icon, LV_SYMBOL_EYE_OPEN);
    lv_obj_center(eye_icon);

    // Navigation
    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_add_style(btn_back, &style_wizard_btn_secondary, 0);
    lv_obj_set_size(btn_back, 120, 45);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 50, 155);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        showWizardStep(WIZARD_WIFI);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *back_label = lv_label_create(btn_back);
    char back_text[32];
    snprintf(back_text, sizeof(back_text), LV_SYMBOL_LEFT " %s", tr(STR_BACK));
    lv_label_set_text(back_label, back_text);
    lv_obj_center(back_label);

    lv_obj_t *btn_next = lv_btn_create(scr);
    lv_obj_add_style(btn_next, &style_wizard_btn_primary, 0);
    lv_obj_set_size(btn_next, 150, 45);
    lv_obj_align(btn_next, LV_ALIGN_TOP_RIGHT, -50, 155);
    lv_obj_add_event_cb(btn_next, [](lv_event_t *e) {
        const char *pwd = lv_textarea_get_text(pwd_textarea);
        strlcpy(appConfig.wifi[currentWifiSlot].password, pwd,
                sizeof(appConfig.wifi[currentWifiSlot].password));
        appConfig.wifi[currentWifiSlot].enabled = true;
        if (currentWifiSlot < 2) {
            showWizardStep(WIZARD_WIFI_ADD_MORE);
        } else {
            showWizardStep(WIZARD_API_KEY);
        }
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *next_label = lv_label_create(btn_next);
    char next_text[32];
    snprintf(next_text, sizeof(next_text), "%s " LV_SYMBOL_RIGHT, tr(STR_NEXT));
    lv_label_set_text(next_label, next_text);
    lv_obj_center(next_label);

    lv_obj_t *kb = lv_keyboard_create(scr);
    lv_keyboard_set_textarea(kb, pwd_textarea);
    lv_obj_set_size(kb, SCREEN_WIDTH, 280);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Step 3b: Add More WiFi?
// ============================================================================

void showWiFiAddMoreStep() {
    lv_obj_t *scr = createWizardScreen(tr(STR_WIFI_CONFIGURED), "", 1, 6);

    lv_obj_t *icon = lv_label_create(scr);
    lv_label_set_text(icon, LV_SYMBOL_OK);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x4ECDC4), 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -80);

    char msg[128];
    snprintf(msg, sizeof(msg), "%s %d:\n%s", tr(STR_WIFI_TITLE), currentWifiSlot + 1,
             appConfig.wifi[currentWifiSlot].ssid);
    lv_obj_t *lbl_msg = lv_label_create(scr);
    lv_label_set_text(lbl_msg, msg);
    lv_obj_set_style_text_color(lbl_msg, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_msg, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(lbl_msg, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(lbl_msg, LV_ALIGN_CENTER, 0, -10);

    char networks_msg[64];
    snprintf(networks_msg, sizeof(networks_msg), tr(STR_WIFI_NETWORKS_CONFIGURED), currentWifiSlot + 1);
    lv_obj_t *lbl_networks = lv_label_create(scr);
    lv_label_set_text(lbl_networks, networks_msg);
    lv_obj_set_style_text_color(lbl_networks, lv_color_hex(0xaaaaaa), 0);
    lv_obj_align(lbl_networks, LV_ALIGN_CENTER, 0, 40);

    lv_obj_t *btn_add = lv_btn_create(scr);
    lv_obj_add_style(btn_add, &style_wizard_btn_secondary, 0);
    lv_obj_set_size(btn_add, 220, 50);
    lv_obj_align(btn_add, LV_ALIGN_CENTER, -130, 110);
    lv_obj_add_event_cb(btn_add, [](lv_event_t *e) {
        currentWifiSlot++;
        wifiCount = 0;
        showWizardStep(WIZARD_WIFI_SCANNING);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *add_label = lv_label_create(btn_add);
    char add_text[48];
    snprintf(add_text, sizeof(add_text), LV_SYMBOL_PLUS " %s", tr(STR_WIFI_ADD_ANOTHER));
    lv_label_set_text(add_label, add_text);
    lv_obj_center(add_label);

    lv_obj_t *btn_continue = lv_btn_create(scr);
    lv_obj_add_style(btn_continue, &style_wizard_btn_primary, 0);
    lv_obj_set_size(btn_continue, 180, 50);
    lv_obj_align(btn_continue, LV_ALIGN_CENTER, 130, 110);
    lv_obj_add_event_cb(btn_continue, [](lv_event_t *e) {
        showWizardStep(WIZARD_API_KEY);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *continue_label = lv_label_create(btn_continue);
    char continue_text[32];
    snprintf(continue_text, sizeof(continue_text), "%s " LV_SYMBOL_RIGHT, tr(STR_CONTINUE));
    lv_label_set_text(continue_label, continue_text);
    lv_obj_center(continue_label);

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Step 4: API Key
// ============================================================================

static lv_obj_t *api_textarea = nullptr;

void showAPIKeyStep() {
    lv_obj_t *scr = createWizardScreen(tr(STR_API_KEY_TITLE), "WeatherAPI.com", 2, 6);

    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, tr(STR_API_KEY_DESC));
    lv_obj_set_style_text_color(info, lv_color_hex(0xaaaaaa), 0);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_16, 0);
    lv_obj_align(info, LV_ALIGN_TOP_MID, 0, 95);

    api_textarea = lv_textarea_create(scr);
    lv_obj_set_size(api_textarea, SCREEN_WIDTH - 200, 50);
    lv_obj_align(api_textarea, LV_ALIGN_TOP_MID, 0, 150);
    lv_textarea_set_placeholder_text(api_textarea, tr(STR_API_KEY_PLACEHOLDER));
    lv_textarea_set_one_line(api_textarea, true);
    lv_textarea_set_max_length(api_textarea, 64);
    if (strlen(appConfig.api.weather_api_key) > 0) {
        lv_textarea_set_text(api_textarea, appConfig.api.weather_api_key);
    }

    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_add_style(btn_back, &style_wizard_btn_secondary, 0);
    lv_obj_set_size(btn_back, 120, 45);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 50, 210);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        showWizardStep(WIZARD_WIFI);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *back_label = lv_label_create(btn_back);
    char back_text[32];
    snprintf(back_text, sizeof(back_text), LV_SYMBOL_LEFT " %s", tr(STR_BACK));
    lv_label_set_text(back_label, back_text);
    lv_obj_center(back_label);

    lv_obj_t *btn_next = lv_btn_create(scr);
    lv_obj_add_style(btn_next, &style_wizard_btn_primary, 0);
    lv_obj_set_size(btn_next, 150, 45);
    lv_obj_align(btn_next, LV_ALIGN_TOP_RIGHT, -50, 210);
    lv_obj_add_event_cb(btn_next, [](lv_event_t *e) {
        const char *key = lv_textarea_get_text(api_textarea);
        strlcpy(appConfig.api.weather_api_key, key, sizeof(appConfig.api.weather_api_key));
        showWizardStep(WIZARD_LOCATION);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *next_label = lv_label_create(btn_next);
    char next_text[32];
    snprintf(next_text, sizeof(next_text), "%s " LV_SYMBOL_RIGHT, tr(STR_NEXT));
    lv_label_set_text(next_label, next_text);
    lv_obj_center(next_label);

    lv_obj_t *kb = lv_keyboard_create(scr);
    lv_keyboard_set_textarea(kb, api_textarea);
    lv_obj_set_size(kb, SCREEN_WIDTH, 280);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Step 5: Location
// ============================================================================

static lv_obj_t *loc_search_textarea = nullptr;
static lv_obj_t *loc_results_cont = nullptr;
static lv_obj_t *loc_lat_textarea = nullptr;
static lv_obj_t *loc_lon_textarea = nullptr;
static lv_obj_t *loc_keyboard = nullptr;
static bool loc_manual_mode = false;

void updateLocationResults() {
    lv_obj_clean(loc_results_cont);

    if (locationCount == 0) {
        lv_obj_t *no_results = lv_label_create(loc_results_cont);
        lv_label_set_text(no_results, tr(STR_LOCATION_NO_RESULTS));
        lv_obj_set_style_text_color(no_results, lv_color_hex(0xff6666), 0);
        return;
    }

    for (int i = 0; i < locationCount; i++) {
        lv_obj_t *item = lv_btn_create(loc_results_cont);
        lv_obj_add_style(item, &style_wizard_list_item, 0);
        lv_obj_set_size(item, lv_pct(100), 55);
        lv_obj_set_user_data(item, (void*)(intptr_t)i);

        lv_obj_add_event_cb(item, [](lv_event_t *e) {
            int idx = (int)(intptr_t)lv_obj_get_user_data(lv_event_get_target(e));
            char lat_str[16], lon_str[16];
            snprintf(lat_str, sizeof(lat_str), "%.4f", locationLats[idx]);
            snprintf(lon_str, sizeof(lon_str), "%.4f", locationLons[idx]);
            strlcpy(appConfig.location.latitude, lat_str, sizeof(appConfig.location.latitude));
            strlcpy(appConfig.location.longitude, lon_str, sizeof(appConfig.location.longitude));
            strlcpy(appConfig.location.city, locationNames[idx].c_str(), sizeof(appConfig.location.city));
            showWizardStep(WIZARD_DISPLAY);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *icon = lv_label_create(item);
        lv_label_set_text(icon, LV_SYMBOL_GPS);
        lv_obj_set_style_text_color(icon, lv_color_hex(0x4ECDC4), 0);
        lv_obj_align(icon, LV_ALIGN_LEFT_MID, 5, 0);

        lv_obj_t *name = lv_label_create(item);
        lv_label_set_text(name, locationNames[i].c_str());
        lv_obj_set_style_text_font(name, &lv_font_montserrat_18, 0);
        lv_obj_align(name, LV_ALIGN_LEFT_MID, 40, -8);

        lv_obj_t *region = lv_label_create(item);
        lv_label_set_text(region, locationRegions[i].c_str());
        lv_obj_set_style_text_color(region, lv_color_hex(0x888888), 0);
        lv_obj_set_style_text_font(region, &lv_font_montserrat_14, 0);
        lv_obj_align(region, LV_ALIGN_LEFT_MID, 40, 10);
    }
}

void showLocationStep() {
    lv_obj_t *scr = createWizardScreen(tr(STR_LOCATION_TITLE), tr(STR_LOCATION_DESC), 3, 6);

    if (loc_manual_mode) {
        // Manual coordinates mode
        lv_obj_t *lat_label = lv_label_create(scr);
        lv_label_set_text(lat_label, tr(STR_LOCATION_LATITUDE));
        lv_obj_set_style_text_color(lat_label, lv_color_white(), 0);
        lv_obj_align(lat_label, LV_ALIGN_TOP_LEFT, 100, 110);

        loc_lat_textarea = lv_textarea_create(scr);
        lv_obj_set_size(loc_lat_textarea, 180, 45);
        lv_obj_align(loc_lat_textarea, LV_ALIGN_TOP_LEFT, 200, 105);
        lv_textarea_set_placeholder_text(loc_lat_textarea, "19.4326");
        lv_textarea_set_one_line(loc_lat_textarea, true);

        lv_obj_t *lon_label = lv_label_create(scr);
        lv_label_set_text(lon_label, tr(STR_LOCATION_LONGITUDE));
        lv_obj_set_style_text_color(lon_label, lv_color_white(), 0);
        lv_obj_align(lon_label, LV_ALIGN_TOP_LEFT, 420, 110);

        loc_lon_textarea = lv_textarea_create(scr);
        lv_obj_set_size(loc_lon_textarea, 180, 45);
        lv_obj_align(loc_lon_textarea, LV_ALIGN_TOP_LEFT, 530, 105);
        lv_textarea_set_placeholder_text(loc_lon_textarea, "-99.1332");
        lv_textarea_set_one_line(loc_lon_textarea, true);

        lv_obj_t *btn_search_mode = lv_btn_create(scr);
        lv_obj_add_style(btn_search_mode, &style_wizard_btn, 0);
        lv_obj_set_size(btn_search_mode, 200, 40);
        lv_obj_align(btn_search_mode, LV_ALIGN_TOP_MID, 0, 160);
        lv_obj_add_event_cb(btn_search_mode, [](lv_event_t *e) {
            loc_manual_mode = false;
            showWizardStep(WIZARD_LOCATION);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *sm_label = lv_label_create(btn_search_mode);
        char sm_text[48];
        snprintf(sm_text, sizeof(sm_text), LV_SYMBOL_GPS " %s", tr(STR_LOCATION_SEARCH));
        lv_label_set_text(sm_label, sm_text);
        lv_obj_center(sm_label);

        loc_keyboard = lv_keyboard_create(scr);
        lv_keyboard_set_mode(loc_keyboard, LV_KEYBOARD_MODE_NUMBER);
        lv_keyboard_set_textarea(loc_keyboard, loc_lat_textarea);
        lv_obj_set_size(loc_keyboard, SCREEN_WIDTH, 250);
        lv_obj_align(loc_keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);

        lv_obj_add_event_cb(loc_lat_textarea, [](lv_event_t *e) {
            lv_keyboard_set_textarea(loc_keyboard, loc_lat_textarea);
        }, LV_EVENT_FOCUSED, NULL);

        lv_obj_add_event_cb(loc_lon_textarea, [](lv_event_t *e) {
            lv_keyboard_set_textarea(loc_keyboard, loc_lon_textarea);
        }, LV_EVENT_FOCUSED, NULL);

        // Navigation for manual mode
        lv_obj_t *btn_back = lv_btn_create(scr);
        lv_obj_add_style(btn_back, &style_wizard_btn_secondary, 0);
        lv_obj_set_size(btn_back, 120, 40);
        lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 50, 210);
        lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
            showWizardStep(WIZARD_API_KEY);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *back_label = lv_label_create(btn_back);
        char back_text[32];
        snprintf(back_text, sizeof(back_text), LV_SYMBOL_LEFT " %s", tr(STR_BACK));
        lv_label_set_text(back_label, back_text);
        lv_obj_center(back_label);

        lv_obj_t *btn_next = lv_btn_create(scr);
        lv_obj_add_style(btn_next, &style_wizard_btn_primary, 0);
        lv_obj_set_size(btn_next, 150, 40);
        lv_obj_align(btn_next, LV_ALIGN_TOP_RIGHT, -50, 210);
        lv_obj_add_event_cb(btn_next, [](lv_event_t *e) {
            strlcpy(appConfig.location.latitude, lv_textarea_get_text(loc_lat_textarea),
                    sizeof(appConfig.location.latitude));
            strlcpy(appConfig.location.longitude, lv_textarea_get_text(loc_lon_textarea),
                    sizeof(appConfig.location.longitude));
            showWizardStep(WIZARD_DISPLAY);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *next_label = lv_label_create(btn_next);
        char next_text[32];
        snprintf(next_text, sizeof(next_text), "%s " LV_SYMBOL_RIGHT, tr(STR_NEXT));
        lv_label_set_text(next_label, next_text);
        lv_obj_center(next_label);
    } else {
        // Search mode
        loc_search_textarea = lv_textarea_create(scr);
        lv_obj_set_size(loc_search_textarea, SCREEN_WIDTH - 380, 45);
        lv_obj_align(loc_search_textarea, LV_ALIGN_TOP_LEFT, 50, 95);
        lv_textarea_set_placeholder_text(loc_search_textarea, tr(STR_LOCATION_SEARCH_PLACEHOLDER));
        lv_textarea_set_one_line(loc_search_textarea, true);

        lv_obj_t *btn_search = lv_btn_create(scr);
        lv_obj_add_style(btn_search, &style_wizard_btn_primary, 0);
        lv_obj_set_size(btn_search, 120, 45);
        lv_obj_align_to(btn_search, loc_search_textarea, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        lv_obj_add_event_cb(btn_search, [](lv_event_t *e) {
            const char *query = lv_textarea_get_text(loc_search_textarea);
            if (strlen(query) < 2) return;
            if (WiFi.status() != WL_CONNECTED) connectToConfiguredWiFi();
            if (searchLocations(query)) updateLocationResults();
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *search_label = lv_label_create(btn_search);
        char search_text[32];
        snprintf(search_text, sizeof(search_text), LV_SYMBOL_GPS " %s", tr(STR_SEARCH));
        lv_label_set_text(search_label, search_text);
        lv_obj_center(search_label);

        lv_obj_t *btn_manual = lv_btn_create(scr);
        lv_obj_add_style(btn_manual, &style_wizard_btn, 0);
        lv_obj_set_size(btn_manual, 150, 45);
        lv_obj_align_to(btn_manual, btn_search, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
        lv_obj_add_event_cb(btn_manual, [](lv_event_t *e) {
            loc_manual_mode = true;
            showWizardStep(WIZARD_LOCATION);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *manual_label = lv_label_create(btn_manual);
        lv_label_set_text(manual_label, tr(STR_LOCATION_MANUAL));
        lv_obj_center(manual_label);

        loc_results_cont = lv_obj_create(scr);
        lv_obj_set_size(loc_results_cont, SCREEN_WIDTH - 100, 280);
        lv_obj_align(loc_results_cont, LV_ALIGN_TOP_MID, 0, 150);
        lv_obj_add_style(loc_results_cont, &style_wizard_card, 0);
        lv_obj_set_flex_flow(loc_results_cont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(loc_results_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(loc_results_cont, 8, 0);
        lv_obj_set_scrollbar_mode(loc_results_cont, LV_SCROLLBAR_MODE_AUTO);

        lv_obj_t *hint = lv_label_create(loc_results_cont);
        lv_label_set_text(hint, tr(STR_LOCATION_SEARCH_HINT));
        lv_obj_set_style_text_color(hint, lv_color_hex(0x888888), 0);

        lv_obj_t *btn_back = lv_btn_create(scr);
        lv_obj_add_style(btn_back, &style_wizard_btn_secondary, 0);
        lv_obj_set_size(btn_back, 120, 45);
        lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 50, -25);
        lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
            showWizardStep(WIZARD_API_KEY);
        }, LV_EVENT_CLICKED, NULL);

        lv_obj_t *back_label = lv_label_create(btn_back);
        char back_text[32];
        snprintf(back_text, sizeof(back_text), LV_SYMBOL_LEFT " %s", tr(STR_BACK));
        lv_label_set_text(back_label, back_text);
        lv_obj_center(back_label);
    }

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Step 6: Display Settings
// ============================================================================

void showDisplayStep() {
    lv_obj_t *scr = createWizardScreen(tr(STR_DISPLAY_TITLE), tr(STR_DISPLAY_DESC), 4, 6);

    int y = 100;

    // Brightness
    lv_obj_t *bright_label = lv_label_create(scr);
    lv_label_set_text(bright_label, tr(STR_DISPLAY_BRIGHTNESS));
    lv_obj_set_style_text_color(bright_label, lv_color_white(), 0);
    lv_obj_align(bright_label, LV_ALIGN_TOP_LEFT, 80, y + 5);

    lv_obj_t *slider = lv_slider_create(scr);
    lv_obj_set_size(slider, 350, 15);
    lv_obj_align(slider, LV_ALIGN_TOP_LEFT, 200, y + 5);
    lv_slider_set_range(slider, 10, 100);
    lv_slider_set_value(slider, appConfig.display.brightness, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x4ECDC4), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x4ECDC4), LV_PART_KNOB);

    lv_obj_t *bright_val = lv_label_create(scr);
    char bright_str[8];
    snprintf(bright_str, sizeof(bright_str), "%d%%", appConfig.display.brightness);
    lv_label_set_text(bright_val, bright_str);
    lv_obj_set_style_text_color(bright_val, lv_color_white(), 0);
    lv_obj_align(bright_val, LV_ALIGN_TOP_LEFT, 570, y + 5);

    lv_obj_set_user_data(slider, bright_val);
    lv_obj_add_event_cb(slider, [](lv_event_t *e) {
        lv_obj_t *s = lv_event_get_target(e);
        int val = lv_slider_get_value(s);
        appConfig.display.brightness = val;
        lv_obj_t *lbl = (lv_obj_t*)lv_obj_get_user_data(s);
        char str[8];
        snprintf(str, sizeof(str), "%d%%", val);
        lv_label_set_text(lbl, str);
    }, LV_EVENT_VALUE_CHANGED, NULL);

    y += 55;

    // Update interval
    lv_obj_t *interval_label = lv_label_create(scr);
    lv_label_set_text(interval_label, tr(STR_DISPLAY_UPDATE_INTERVAL));
    lv_obj_set_style_text_color(interval_label, lv_color_white(), 0);
    lv_obj_align(interval_label, LV_ALIGN_TOP_LEFT, 80, y + 8);

    lv_obj_t *interval_dd = lv_dropdown_create(scr);
    lv_dropdown_set_options(interval_dd, "5 min\n10 min\n15 min\n30 min\n60 min");
    lv_obj_set_size(interval_dd, 130, 40);
    lv_obj_align(interval_dd, LV_ALIGN_TOP_LEFT, 250, y);

    int intervals[] = {5, 10, 15, 30, 60};
    for (int i = 0; i < 5; i++) {
        if (appConfig.display.update_interval == intervals[i]) {
            lv_dropdown_set_selected(interval_dd, i);
            break;
        }
    }

    lv_obj_add_event_cb(interval_dd, [](lv_event_t *e) {
        int intervals[] = {5, 10, 15, 30, 60};
        appConfig.display.update_interval = intervals[lv_dropdown_get_selected(lv_event_get_target(e))];
    }, LV_EVENT_VALUE_CHANGED, NULL);

    y += 55;

    // Units
    lv_obj_t *units_label = lv_label_create(scr);
    lv_label_set_text(units_label, tr(STR_DISPLAY_UNITS));
    lv_obj_set_style_text_color(units_label, lv_color_white(), 0);
    lv_obj_align(units_label, LV_ALIGN_TOP_LEFT, 80, y + 8);

    lv_obj_t *units_dd = lv_dropdown_create(scr);
    char units_opts[64];
    snprintf(units_opts, sizeof(units_opts), "%s\n%s", tr(STR_DISPLAY_UNITS_METRIC), tr(STR_DISPLAY_UNITS_IMPERIAL));
    lv_dropdown_set_options(units_dd, units_opts);
    lv_obj_set_size(units_dd, 200, 40);
    lv_obj_align(units_dd, LV_ALIGN_TOP_LEFT, 180, y);
    lv_dropdown_set_selected(units_dd, appConfig.display.use_metric ? 0 : 1);

    lv_obj_add_event_cb(units_dd, [](lv_event_t *e) {
        appConfig.display.use_metric = (lv_dropdown_get_selected(lv_event_get_target(e)) == 0);
    }, LV_EVENT_VALUE_CHANGED, NULL);

    y += 55;

    // Time format
    lv_obj_t *time_label = lv_label_create(scr);
    lv_label_set_text(time_label, tr(STR_DISPLAY_TIME_FORMAT));
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_TOP_LEFT, 80, y + 8);

    lv_obj_t *time_dd = lv_dropdown_create(scr);
    char time_opts[64];
    snprintf(time_opts, sizeof(time_opts), "%s\n%s", tr(STR_DISPLAY_24H), tr(STR_DISPLAY_12H));
    lv_dropdown_set_options(time_dd, time_opts);
    lv_obj_set_size(time_dd, 200, 40);
    lv_obj_align(time_dd, LV_ALIGN_TOP_LEFT, 220, y);
    lv_dropdown_set_selected(time_dd, appConfig.display.use_24h ? 0 : 1);

    lv_obj_add_event_cb(time_dd, [](lv_event_t *e) {
        appConfig.display.use_24h = (lv_dropdown_get_selected(lv_event_get_target(e)) == 0);
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Navigation
    lv_obj_t *btn_back = lv_btn_create(scr);
    lv_obj_add_style(btn_back, &style_wizard_btn_secondary, 0);
    lv_obj_set_size(btn_back, 120, 50);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 50, -30);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        showWizardStep(WIZARD_LOCATION);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *back_label = lv_label_create(btn_back);
    char back_text[32];
    snprintf(back_text, sizeof(back_text), LV_SYMBOL_LEFT " %s", tr(STR_BACK));
    lv_label_set_text(back_label, back_text);
    lv_obj_center(back_label);

    lv_obj_t *btn_finish = lv_btn_create(scr);
    lv_obj_add_style(btn_finish, &style_wizard_btn_primary, 0);
    lv_obj_set_size(btn_finish, 180, 50);
    lv_obj_align(btn_finish, LV_ALIGN_BOTTOM_RIGHT, -50, -30);
    lv_obj_add_event_cb(btn_finish, [](lv_event_t *e) {
        showWizardStep(WIZARD_COMPLETE);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *finish_label = lv_label_create(btn_finish);
    char finish_text[32];
    snprintf(finish_text, sizeof(finish_text), "%s " LV_SYMBOL_OK, tr(STR_FINISH));
    lv_label_set_text(finish_label, finish_text);
    lv_obj_set_style_text_font(finish_label, &lv_font_montserrat_18, 0);
    lv_obj_center(finish_label);

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Step 7: Complete
// ============================================================================

void showCompleteStep() {
    lv_obj_t *scr = createWizardScreen(tr(STR_COMPLETE_TITLE), "", 6, 6);

    lv_obj_t *icon = lv_label_create(scr);
    lv_label_set_text(icon, LV_SYMBOL_OK);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0x4ECDC4), 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -100);

    lv_obj_t *msg = lv_label_create(scr);
    lv_label_set_text(msg, tr(STR_COMPLETE_MSG));
    lv_obj_set_style_text_font(msg, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(msg, lv_color_white(), 0);
    lv_obj_align(msg, LV_ALIGN_CENTER, 0, -40);

    char summary[256];
    snprintf(summary, sizeof(summary), "%s: %s\n%s: %s\n%s: %d %s\n%s: %s",
        tr(STR_COMPLETE_SUMMARY_WIFI), appConfig.wifi[0].ssid,
        tr(STR_COMPLETE_SUMMARY_LOCATION),
        strlen(appConfig.location.city) > 0 ? appConfig.location.city : appConfig.location.latitude,
        tr(STR_COMPLETE_SUMMARY_UPDATE), appConfig.display.update_interval, tr(STR_MINUTES),
        tr(STR_COMPLETE_SUMMARY_UNITS), appConfig.display.use_metric ? tr(STR_METRIC) : tr(STR_IMPERIAL));

    lv_obj_t *sum_lbl = lv_label_create(scr);
    lv_label_set_text(sum_lbl, summary);
    lv_obj_set_style_text_color(sum_lbl, lv_color_hex(0xaaaaaa), 0);
    lv_obj_set_style_text_align(sum_lbl, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(sum_lbl, LV_ALIGN_CENTER, 0, 40);

    appConfig.setup_complete = true;
    saveConfig();

    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_add_style(btn, &style_wizard_btn_primary, 0);
    lv_obj_set_size(btn, 250, 55);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -40);

    lv_obj_t *btn_label = lv_label_create(btn);
    char btn_text[32];
    snprintf(btn_text, sizeof(btn_text), "%s " LV_SYMBOL_RIGHT, tr(STR_START));
    lv_label_set_text(btn_label, btn_text);
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_18, 0);
    lv_obj_center(btn_label);

    lv_scr_load(scr);
    currentScreen = scr;
}

// ============================================================================
// Show Wizard Step
// ============================================================================

void showWizardStep(WizardStep step) {
    Serial.printf("showWizardStep(%d)\n", step);

    lv_obj_t *old_scr = currentScreen;
    currentWizardStep = step;

    // Create and load new screen (each function sets currentScreen)
    switch (step) {
        case WIZARD_LANGUAGE: showLanguageStep(); break;
        case WIZARD_WELCOME: showWelcomeStep(); break;
        case WIZARD_WIFI_SCANNING: showWiFiScanningStep(); break;
        case WIZARD_WIFI: showWiFiStep(); break;
        case WIZARD_WIFI_PASSWORD: showWiFiPasswordStep(); break;
        case WIZARD_WIFI_ADD_MORE: showWiFiAddMoreStep(); break;
        case WIZARD_API_KEY: showAPIKeyStep(); break;
        case WIZARD_LOCATION: showLocationStep(); break;
        case WIZARD_DISPLAY: showDisplayStep(); break;
        case WIZARD_COMPLETE: showCompleteStep(); break;
    }

    // Delete old screen AFTER new one is loaded (synchronously)
    if (old_scr != nullptr && old_scr != currentScreen) {
        lv_obj_del(old_scr);
    }
}

// ============================================================================
// Stubs
// ============================================================================

void updateLoopCounter() {}
void processWizardTransition() {
    // Handle deferred WiFi scan
    if (wifiScanPending) {
        wifiScanPending = false;
        Serial.println("Performing deferred WiFi scan...");
        initialWiFiScan();
        Serial.println("Scan complete, showing results");
        showWizardStep(WIZARD_WIFI);
    }
}

// ============================================================================
// Start Wizard
// ============================================================================

void startSetupWizard() {
    Serial.println("=== Starting Setup Wizard ===");
    initWizardStyles();
    setConfigDefaults();
    showWizardStep(WIZARD_LANGUAGE);
    Serial.println("Wizard started");
}

bool shouldRunWizard() {
    // If hardcoded config exists (my_config.h has valid API key), skip wizard
    if (strlen(MY_WEATHER_API_KEY) > 10) {
        Serial.println("Hardcoded config found, skipping wizard");
        return false;
    }

    // Otherwise check Preferences
    Preferences prefs;
    prefs.begin("config", true);
    bool complete = prefs.getBool("setup_complete", false);
    prefs.end();
    return !complete;
}

#endif // SETUP_WIZARD_H
