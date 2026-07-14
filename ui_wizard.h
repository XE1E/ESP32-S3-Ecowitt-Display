/**
 * ui_wizard.h - Wizard de configuracion inicial
 *
 * Se muestra cuando:
 * - Primera ejecucion (no hay WiFi guardado)
 * - Usuario solicita reconfiguracion
 */

#ifndef UI_WIZARD_H
#define UI_WIZARD_H

#include <Arduino.h>
#include <lvgl.h>
#include <WiFi.h>
#include <Preferences.h>
#include "config.h"

// Forward declarations
void showWelcomeStep();
void showWifiScanStep();
void showPasswordStep();
void showServerUrlStep();
void showConnectingStep();
void showSuccessStep();
void showFailedStep(const char *reason);
void saveConfiguration();
void closeWizard(bool success);

// ============================================================================
// Estados del Wizard
// ============================================================================

typedef enum {
    WIZARD_WELCOME = 0,
    WIZARD_WIFI_SCAN,
    WIZARD_WIFI_PASSWORD,
    WIZARD_SERVER_URL,
    WIZARD_CONNECTING,
    WIZARD_SUCCESS,
    WIZARD_FAILED
} WizardStep;

static WizardStep wizardStep = WIZARD_WELCOME;
static lv_obj_t *scr_wizard = nullptr;
static lv_obj_t *wizard_content = nullptr;

// WiFi scan results
#define MAX_NETWORKS 15
static String scannedNetworks[MAX_NETWORKS];
static int32_t scannedRSSI[MAX_NETWORKS];
static int networkCount = 0;
static int selectedNetwork = -1;
static char enteredPassword[64] = {0};
static char enteredServerURL[128] = {0};

// UI elements
static lv_obj_t *keyboard = nullptr;
static lv_obj_t *textarea_active = nullptr;

// Callbacks
typedef void (*WizardCompleteCallback)(bool success);
static WizardCompleteCallback onWizardComplete = nullptr;

// ============================================================================
// Estilos del Wizard
// ============================================================================

static lv_style_t style_wizard_card;
static lv_style_t style_wizard_btn;
static lv_style_t style_wizard_btn_primary;
static bool wizard_styles_init = false;

void initWizardStyles() {
    if (wizard_styles_init) return;

    // Card style
    lv_style_init(&style_wizard_card);
    lv_style_set_bg_color(&style_wizard_card, lv_color_white());
    lv_style_set_bg_opa(&style_wizard_card, LV_OPA_COVER);
    lv_style_set_radius(&style_wizard_card, 20);
    lv_style_set_shadow_width(&style_wizard_card, 20);
    lv_style_set_shadow_color(&style_wizard_card, lv_color_black());
    lv_style_set_shadow_opa(&style_wizard_card, LV_OPA_20);
    lv_style_set_pad_all(&style_wizard_card, 30);

    // Button style
    lv_style_init(&style_wizard_btn);
    lv_style_set_bg_color(&style_wizard_btn, lv_color_hex(0xE0E0E0));
    lv_style_set_radius(&style_wizard_btn, 12);
    lv_style_set_shadow_width(&style_wizard_btn, 0);

    // Primary button
    lv_style_init(&style_wizard_btn_primary);
    lv_style_set_bg_color(&style_wizard_btn_primary, lv_color_hex(0x2196F3));
    lv_style_set_radius(&style_wizard_btn_primary, 12);
    lv_style_set_shadow_width(&style_wizard_btn_primary, 0);

    wizard_styles_init = true;
}

// ============================================================================
// Limpiar contenido actual
// ============================================================================

void clearWizardContent() {
    if (wizard_content) {
        lv_obj_clean(wizard_content);
    }
    if (keyboard) {
        lv_obj_del(keyboard);
        keyboard = nullptr;
    }
    textarea_active = nullptr;
}

// ============================================================================
// Step 1: Bienvenida
// ============================================================================

void showWelcomeStep() {
    clearWizardContent();
    wizardStep = WIZARD_WELCOME;

    // Logo/Icono
    lv_obj_t *icon = lv_label_create(wizard_content);
    lv_label_set_text(icon, LV_SYMBOL_HOME);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_align(icon, LV_ALIGN_TOP_MID, 0, 20);

    // Titulo
    lv_obj_t *title = lv_label_create(wizard_content);
    lv_label_set_text(title, "Ecowitt Display");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x2C2C2C), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 90);

    // Subtitulo
    lv_obj_t *subtitle = lv_label_create(wizard_content);
    lv_label_set_text(subtitle, "Configuracion inicial");
    lv_obj_set_style_text_font(subtitle, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0x666666), 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 130);

    // Descripcion
    lv_obj_t *desc = lv_label_create(wizard_content);
    lv_label_set_text(desc,
        "Este asistente te ayudara a configurar:\n\n"
        "  • Conexion WiFi\n"
        "  • URL del servidor Ecowitt\n"
        "  • Preferencias de visualizacion");
    lv_obj_set_style_text_font(desc, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(desc, lv_color_hex(0x666666), 0);
    lv_obj_set_width(desc, 400);
    lv_obj_align(desc, LV_ALIGN_CENTER, 0, 20);

    // Boton comenzar
    lv_obj_t *btn_start = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_start, 200, 50);
    lv_obj_align(btn_start, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_style(btn_start, &style_wizard_btn_primary, 0);
    lv_obj_add_event_cb(btn_start, [](lv_event_t *e) {
        showWifiScanStep();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn_start);
    lv_label_set_text(btn_label, "Comenzar " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(btn_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_18, 0);
    lv_obj_center(btn_label);

    Serial.println("[WIZARD] Step: Welcome");
}

// ============================================================================
// Step 2: Escanear WiFi
// ============================================================================

static lv_obj_t *wifi_list = nullptr;
static lv_obj_t *lbl_scanning = nullptr;

void updateWifiList() {
    if (!wifi_list) return;

    lv_obj_clean(wifi_list);

    for (int i = 0; i < networkCount; i++) {
        lv_obj_t *btn = lv_btn_create(wifi_list);
        lv_obj_set_size(btn, lv_pct(100), 50);
        lv_obj_add_style(btn, &style_wizard_btn, 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xF5F5F5), 0);

        // SSID
        lv_obj_t *lbl_ssid = lv_label_create(btn);
        lv_label_set_text(lbl_ssid, scannedNetworks[i].c_str());
        lv_obj_set_style_text_font(lbl_ssid, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(lbl_ssid, lv_color_hex(0x2C2C2C), 0);
        lv_obj_align(lbl_ssid, LV_ALIGN_LEFT_MID, 10, 0);

        // Signal strength icon
        lv_obj_t *lbl_signal = lv_label_create(btn);
        const char *signal_icon = scannedRSSI[i] > -50 ? LV_SYMBOL_WIFI :
                                  scannedRSSI[i] > -70 ? LV_SYMBOL_WIFI : LV_SYMBOL_WIFI;
        char signal_txt[32];
        snprintf(signal_txt, sizeof(signal_txt), "%s %d dBm", signal_icon, scannedRSSI[i]);
        lv_label_set_text(lbl_signal, signal_txt);
        lv_obj_set_style_text_font(lbl_signal, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_color(lbl_signal, lv_color_hex(0x999999), 0);
        lv_obj_align(lbl_signal, LV_ALIGN_RIGHT_MID, -10, 0);

        // Store index
        lv_obj_set_user_data(btn, (void*)(intptr_t)i);

        lv_obj_add_event_cb(btn, [](lv_event_t *e) {
            lv_obj_t *btn = lv_event_get_target(e);
            selectedNetwork = (int)(intptr_t)lv_obj_get_user_data(btn);
            showPasswordStep();
        }, LV_EVENT_CLICKED, NULL);
    }
}

void scanWifiNetworks() {
    if (lbl_scanning) {
        lv_label_set_text(lbl_scanning, "Buscando redes WiFi...");
    }

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    int n = WiFi.scanNetworks();
    networkCount = min(n, MAX_NETWORKS);

    for (int i = 0; i < networkCount; i++) {
        scannedNetworks[i] = WiFi.SSID(i);
        scannedRSSI[i] = WiFi.RSSI(i);
    }

    // Ordenar por senal
    for (int i = 0; i < networkCount - 1; i++) {
        for (int j = i + 1; j < networkCount; j++) {
            if (scannedRSSI[j] > scannedRSSI[i]) {
                String tmpS = scannedNetworks[i];
                int32_t tmpR = scannedRSSI[i];
                scannedNetworks[i] = scannedNetworks[j];
                scannedRSSI[i] = scannedRSSI[j];
                scannedNetworks[j] = tmpS;
                scannedRSSI[j] = tmpR;
            }
        }
    }

    if (lbl_scanning) {
        if (networkCount > 0) {
            char txt[32];
            snprintf(txt, sizeof(txt), "%d redes encontradas", networkCount);
            lv_label_set_text(lbl_scanning, txt);
        } else {
            lv_label_set_text(lbl_scanning, "No se encontraron redes");
        }
    }

    updateWifiList();
    Serial.printf("[WIZARD] Scan: %d networks\n", networkCount);
}

void showWifiScanStep() {
    clearWizardContent();
    wizardStep = WIZARD_WIFI_SCAN;

    // Titulo
    lv_obj_t *title = lv_label_create(wizard_content);
    lv_label_set_text(title, LV_SYMBOL_WIFI " Selecciona tu red WiFi");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x2C2C2C), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Status
    lbl_scanning = lv_label_create(wizard_content);
    lv_label_set_text(lbl_scanning, "Buscando redes WiFi...");
    lv_obj_set_style_text_font(lbl_scanning, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_scanning, lv_color_hex(0x666666), 0);
    lv_obj_align(lbl_scanning, LV_ALIGN_TOP_MID, 0, 50);

    // Lista de redes
    wifi_list = lv_obj_create(wizard_content);
    lv_obj_set_size(wifi_list, 500, 280);
    lv_obj_align(wifi_list, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_style_bg_opa(wifi_list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(wifi_list, 0, 0);
    lv_obj_set_flex_flow(wifi_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(wifi_list, 8, 0);

    // Botones
    lv_obj_t *btn_back = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_back, 120, 45);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_LEFT, 20, -10);
    lv_obj_add_style(btn_back, &style_wizard_btn, 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        showWelcomeStep();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, LV_SYMBOL_LEFT " Atras");
    lv_obj_center(lbl_back);

    lv_obj_t *btn_refresh = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_refresh, 150, 45);
    lv_obj_align(btn_refresh, LV_ALIGN_BOTTOM_RIGHT, -20, -10);
    lv_obj_add_style(btn_refresh, &style_wizard_btn_primary, 0);
    lv_obj_add_event_cb(btn_refresh, [](lv_event_t *e) {
        scanWifiNetworks();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_refresh = lv_label_create(btn_refresh);
    lv_label_set_text(lbl_refresh, LV_SYMBOL_REFRESH " Escanear");
    lv_obj_set_style_text_color(lbl_refresh, lv_color_white(), 0);
    lv_obj_center(lbl_refresh);

    // Iniciar escaneo
    scanWifiNetworks();

    Serial.println("[WIZARD] Step: WiFi Scan");
}

// ============================================================================
// Step 3: Contrasena WiFi
// ============================================================================

static lv_obj_t *ta_password = nullptr;

void showPasswordStep() {
    clearWizardContent();
    wizardStep = WIZARD_WIFI_PASSWORD;

    // Titulo
    lv_obj_t *title = lv_label_create(wizard_content);
    lv_label_set_text(title, LV_SYMBOL_WIFI " Contrasena WiFi");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x2C2C2C), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Red seleccionada
    lv_obj_t *lbl_network = lv_label_create(wizard_content);
    char txt[64];
    snprintf(txt, sizeof(txt), "Red: %s", scannedNetworks[selectedNetwork].c_str());
    lv_label_set_text(lbl_network, txt);
    lv_obj_set_style_text_font(lbl_network, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_network, lv_color_hex(0x666666), 0);
    lv_obj_align(lbl_network, LV_ALIGN_TOP_MID, 0, 50);

    // Campo de contrasena
    ta_password = lv_textarea_create(wizard_content);
    lv_textarea_set_one_line(ta_password, true);
    lv_textarea_set_password_mode(ta_password, true);
    lv_textarea_set_placeholder_text(ta_password, "Ingresa la contrasena");
    lv_obj_set_size(ta_password, 400, 50);
    lv_obj_align(ta_password, LV_ALIGN_TOP_MID, 0, 90);
    lv_obj_set_style_text_font(ta_password, &lv_font_montserrat_18, 0);

    // Checkbox mostrar contrasena
    lv_obj_t *cb_show = lv_checkbox_create(wizard_content);
    lv_checkbox_set_text(cb_show, "Mostrar contrasena");
    lv_obj_align(cb_show, LV_ALIGN_TOP_MID, 0, 150);
    lv_obj_add_event_cb(cb_show, [](lv_event_t *e) {
        lv_obj_t *cb = lv_event_get_target(e);
        bool checked = lv_obj_get_state(cb) & LV_STATE_CHECKED;
        lv_textarea_set_password_mode(ta_password, !checked);
    }, LV_EVENT_VALUE_CHANGED, NULL);

    // Teclado
    keyboard = lv_keyboard_create(scr_wizard);
    lv_keyboard_set_textarea(keyboard, ta_password);
    lv_obj_set_size(keyboard, SCREEN_WIDTH, 280);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Botones
    lv_obj_t *btn_back = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_back, 120, 45);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 0, 180);
    lv_obj_add_style(btn_back, &style_wizard_btn, 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        showWifiScanStep();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, LV_SYMBOL_LEFT " Atras");
    lv_obj_center(lbl_back);

    lv_obj_t *btn_next = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_next, 150, 45);
    lv_obj_align(btn_next, LV_ALIGN_TOP_RIGHT, 0, 180);
    lv_obj_add_style(btn_next, &style_wizard_btn_primary, 0);
    lv_obj_add_event_cb(btn_next, [](lv_event_t *e) {
        const char *pwd = lv_textarea_get_text(ta_password);
        strncpy(enteredPassword, pwd, sizeof(enteredPassword) - 1);
        showServerUrlStep();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_next = lv_label_create(btn_next);
    lv_label_set_text(lbl_next, "Siguiente " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(lbl_next, lv_color_white(), 0);
    lv_obj_center(lbl_next);

    Serial.println("[WIZARD] Step: Password");
}

// ============================================================================
// Step 4: URL del servidor
// ============================================================================

static lv_obj_t *ta_server = nullptr;

void showServerUrlStep() {
    clearWizardContent();
    wizardStep = WIZARD_SERVER_URL;

    // Titulo
    lv_obj_t *title = lv_label_create(wizard_content);
    lv_label_set_text(title, LV_SYMBOL_SETTINGS " Servidor Ecowitt");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x2C2C2C), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Descripcion
    lv_obj_t *desc = lv_label_create(wizard_content);
    lv_label_set_text(desc, "Ingresa la URL de tu servidor Ecowitt\n(ej: https://clima.example.com)");
    lv_obj_set_style_text_font(desc, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(desc, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_align(desc, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(desc, LV_ALIGN_TOP_MID, 0, 50);

    // Campo URL
    ta_server = lv_textarea_create(wizard_content);
    lv_textarea_set_one_line(ta_server, true);
    lv_textarea_set_placeholder_text(ta_server, "https://");
    lv_textarea_set_text(ta_server, "https://clima.xe1e.net");
    lv_obj_set_size(ta_server, 450, 50);
    lv_obj_align(ta_server, LV_ALIGN_TOP_MID, 0, 100);
    lv_obj_set_style_text_font(ta_server, &lv_font_montserrat_16, 0);

    // Teclado
    keyboard = lv_keyboard_create(scr_wizard);
    lv_keyboard_set_textarea(keyboard, ta_server);
    lv_obj_set_size(keyboard, SCREEN_WIDTH, 280);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Botones
    lv_obj_t *btn_back = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_back, 120, 45);
    lv_obj_align(btn_back, LV_ALIGN_TOP_LEFT, 0, 170);
    lv_obj_add_style(btn_back, &style_wizard_btn, 0);
    lv_obj_add_event_cb(btn_back, [](lv_event_t *e) {
        showPasswordStep();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_back = lv_label_create(btn_back);
    lv_label_set_text(lbl_back, LV_SYMBOL_LEFT " Atras");
    lv_obj_center(lbl_back);

    lv_obj_t *btn_connect = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_connect, 150, 45);
    lv_obj_align(btn_connect, LV_ALIGN_TOP_RIGHT, 0, 170);
    lv_obj_add_style(btn_connect, &style_wizard_btn_primary, 0);
    lv_obj_add_event_cb(btn_connect, [](lv_event_t *e) {
        const char *url = lv_textarea_get_text(ta_server);
        strncpy(enteredServerURL, url, sizeof(enteredServerURL) - 1);
        showConnectingStep();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_connect = lv_label_create(btn_connect);
    lv_label_set_text(lbl_connect, "Conectar " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(lbl_connect, lv_color_white(), 0);
    lv_obj_center(lbl_connect);

    Serial.println("[WIZARD] Step: Server URL");
}

// ============================================================================
// Step 5: Conectando
// ============================================================================

static lv_obj_t *spinner = nullptr;
static lv_obj_t *lbl_status = nullptr;
static lv_timer_t *connect_timer = nullptr;

void attemptConnection(lv_timer_t *timer) {
    static int phase = 0;

    switch (phase) {
        case 0:
            lv_label_set_text(lbl_status, "Conectando a WiFi...");
            WiFi.begin(scannedNetworks[selectedNetwork].c_str(), enteredPassword);
            phase++;
            break;

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (WiFi.status() == WL_CONNECTED) {
                lv_label_set_text(lbl_status, "WiFi conectado!");
                phase = 20;
            } else {
                phase++;
            }
            break;

        case 11:
            if (WiFi.status() != WL_CONNECTED) {
                lv_timer_del(timer);
                connect_timer = nullptr;
                showFailedStep("No se pudo conectar a WiFi");
                phase = 0;
                return;
            }
            break;

        case 20:
            lv_label_set_text(lbl_status, "Verificando servidor...");
            phase++;
            break;

        case 21: {
            // Verificar servidor (simplificado)
            extern EcowittAPI ecowittApi;
            bool serverOk = ecowittApi.checkConnection();

            if (serverOk) {
                lv_timer_del(timer);
                connect_timer = nullptr;
                saveConfiguration();
                showSuccessStep();
            } else {
                lv_timer_del(timer);
                connect_timer = nullptr;
                showFailedStep("No se pudo conectar al servidor");
            }
            phase = 0;
            return;
        }
    }
}

void showConnectingStep() {
    clearWizardContent();
    wizardStep = WIZARD_CONNECTING;

    // Spinner
    spinner = lv_spinner_create(wizard_content, 1000, 60);
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_align(spinner, LV_ALIGN_CENTER, 0, -40);

    // Status
    lbl_status = lv_label_create(wizard_content);
    lv_label_set_text(lbl_status, "Conectando...");
    lv_obj_set_style_text_font(lbl_status, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lbl_status, lv_color_hex(0x666666), 0);
    lv_obj_align(lbl_status, LV_ALIGN_CENTER, 0, 50);

    // Info
    lv_obj_t *info = lv_label_create(wizard_content);
    char txt[128];
    snprintf(txt, sizeof(txt), "Red: %s\nServidor: %s",
             scannedNetworks[selectedNetwork].c_str(), enteredServerURL);
    lv_label_set_text(info, txt);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(info, lv_color_hex(0x999999), 0);
    lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 110);

    // Iniciar conexion
    connect_timer = lv_timer_create(attemptConnection, 500, NULL);

    Serial.println("[WIZARD] Step: Connecting");
}

// ============================================================================
// Step 6: Éxito
// ============================================================================

void showSuccessStep() {
    clearWizardContent();
    wizardStep = WIZARD_SUCCESS;

    // Icono
    lv_obj_t *icon = lv_label_create(wizard_content);
    lv_label_set_text(icon, LV_SYMBOL_OK);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -60);

    // Titulo
    lv_obj_t *title = lv_label_create(wizard_content);
    lv_label_set_text(title, "¡Configuracion completa!");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x4CAF50), 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 10);

    // Info
    lv_obj_t *info = lv_label_create(wizard_content);
    char txt[128];
    snprintf(txt, sizeof(txt),
        "Conectado a: %s\n"
        "IP: %s\n"
        "Servidor: OK",
        scannedNetworks[selectedNetwork].c_str(),
        WiFi.localIP().toString().c_str());
    lv_label_set_text(info, txt);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(info, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 80);

    // Boton continuar
    lv_obj_t *btn_done = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_done, 200, 50);
    lv_obj_align(btn_done, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_style(btn_done, &style_wizard_btn_primary, 0);
    lv_obj_add_event_cb(btn_done, [](lv_event_t *e) {
        closeWizard(true);
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_done = lv_label_create(btn_done);
    lv_label_set_text(lbl_done, "Continuar " LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(lbl_done, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_done, &lv_font_montserrat_18, 0);
    lv_obj_center(lbl_done);

    Serial.println("[WIZARD] Step: Success");
}

// ============================================================================
// Step 7: Fallo
// ============================================================================

void showFailedStep(const char *reason) {
    clearWizardContent();
    wizardStep = WIZARD_FAILED;

    // Icono
    lv_obj_t *icon = lv_label_create(wizard_content);
    lv_label_set_text(icon, LV_SYMBOL_CLOSE);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -60);

    // Titulo
    lv_obj_t *title = lv_label_create(wizard_content);
    lv_label_set_text(title, "Error de conexion");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xF44336), 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, 10);

    // Razon
    lv_obj_t *lbl_reason = lv_label_create(wizard_content);
    lv_label_set_text(lbl_reason, reason);
    lv_obj_set_style_text_font(lbl_reason, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_reason, lv_color_hex(0x666666), 0);
    lv_obj_align(lbl_reason, LV_ALIGN_CENTER, 0, 60);

    // Boton reintentar
    lv_obj_t *btn_retry = lv_btn_create(wizard_content);
    lv_obj_set_size(btn_retry, 180, 50);
    lv_obj_align(btn_retry, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_style(btn_retry, &style_wizard_btn_primary, 0);
    lv_obj_add_event_cb(btn_retry, [](lv_event_t *e) {
        showWifiScanStep();
    }, LV_EVENT_CLICKED, NULL);

    lv_obj_t *lbl_retry = lv_label_create(btn_retry);
    lv_label_set_text(lbl_retry, LV_SYMBOL_REFRESH " Reintentar");
    lv_obj_set_style_text_color(lbl_retry, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_retry, &lv_font_montserrat_18, 0);
    lv_obj_center(lbl_retry);

    Serial.printf("[WIZARD] Step: Failed - %s\n", reason);
}

// ============================================================================
// Guardar configuracion en NVS
// ============================================================================

void saveConfiguration() {
    Preferences prefs;
    prefs.begin("ecowitt", false);

    prefs.putString("wifi_ssid", scannedNetworks[selectedNetwork].c_str());
    prefs.putString("wifi_pass", enteredPassword);
    prefs.putString("server_url", enteredServerURL);
    prefs.putBool("configured", true);

    prefs.end();

    Serial.println("[WIZARD] Configuration saved to NVS");
}

// ============================================================================
// Cargar configuracion de NVS
// ============================================================================

bool loadConfiguration(char *ssid, size_t ssid_len,
                       char *pass, size_t pass_len,
                       char *url, size_t url_len) {
    Preferences prefs;
    prefs.begin("ecowitt", true);

    bool configured = prefs.getBool("configured", false);
    if (configured) {
        String s = prefs.getString("wifi_ssid", "");
        String p = prefs.getString("wifi_pass", "");
        String u = prefs.getString("server_url", "https://clima.xe1e.net");

        strncpy(ssid, s.c_str(), ssid_len - 1);
        strncpy(pass, p.c_str(), pass_len - 1);
        strncpy(url, u.c_str(), url_len - 1);
    }

    prefs.end();
    return configured;
}

// ============================================================================
// Mostrar / Cerrar Wizard
// ============================================================================

void showWizard(WizardCompleteCallback callback) {
    onWizardComplete = callback;
    initWizardStyles();

    // Crear pantalla
    scr_wizard = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr_wizard, lv_color_hex(0x1565C0), 0);
    lv_obj_clear_flag(scr_wizard, LV_OBJ_FLAG_SCROLLABLE);

    // Card central
    wizard_content = lv_obj_create(scr_wizard);
    lv_obj_set_size(wizard_content, 600, 420);
    lv_obj_center(wizard_content);
    lv_obj_add_style(wizard_content, &style_wizard_card, 0);
    lv_obj_clear_flag(wizard_content, LV_OBJ_FLAG_SCROLLABLE);

    // Cargar pantalla
    lv_scr_load(scr_wizard);

    // Mostrar bienvenida
    showWelcomeStep();

    Serial.println("[WIZARD] Started");
}

void closeWizard(bool success) {
    if (keyboard) {
        lv_obj_del(keyboard);
        keyboard = nullptr;
    }

    if (connect_timer) {
        lv_timer_del(connect_timer);
        connect_timer = nullptr;
    }

    if (scr_wizard) {
        lv_obj_del(scr_wizard);
        scr_wizard = nullptr;
        wizard_content = nullptr;
    }

    if (onWizardComplete) {
        onWizardComplete(success);
    }

    Serial.printf("[WIZARD] Closed (success=%d)\n", success);
}

// ============================================================================
// Verificar si necesita wizard
// ============================================================================

bool needsWizard() {
    Preferences prefs;
    prefs.begin("ecowitt", true);
    bool configured = prefs.getBool("configured", false);
    prefs.end();
    return !configured;
}

#endif // UI_WIZARD_H
