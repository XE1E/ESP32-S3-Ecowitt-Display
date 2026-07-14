/**
 * ntp_time.h - Independent NTP Time Synchronization
 *
 * Syncs time from atomic clock servers, independent of WeatherAPI.
 * Features:
 * - Multiple NTP server fallback
 * - Automatic periodic resync
 * - Connection status tracking
 * - 24-hour format with timezone support
 */

#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include "config.h"

// ============================================================================
// NTP Configuration
// ============================================================================

// Primary and fallback NTP servers (atomic clock sources)
#define NTP_SERVER_1    "pool.ntp.org"
#define NTP_SERVER_2    "time.nist.gov"
#define NTP_SERVER_3    "time.google.com"

// Sync intervals
#define NTP_SYNC_TIMEOUT_MS     10000   // 10 seconds timeout per attempt
#define NTP_RETRY_DELAY_MS      1000    // 1 second between retries
#define NTP_RESYNC_INTERVAL_MS  3600000 // Resync every 1 hour
#define NTP_MAX_RETRIES         3       // Max retries per server

// ============================================================================
// NTP State
// ============================================================================

enum NTPStatus {
    NTP_STATUS_NOT_SYNCED = 0,
    NTP_STATUS_SYNCING,
    NTP_STATUS_SYNCED,
    NTP_STATUS_SYNC_FAILED
};

static NTPStatus ntpStatus = NTP_STATUS_NOT_SYNCED;
static uint32_t lastSyncTime = 0;
static uint32_t lastSyncAttempt = 0;
static int syncFailCount = 0;

// ============================================================================
// Initialize NTP (call once after WiFi connected)
// ============================================================================

void initNTP() {
    Serial.println("[NTP] Initializing with multiple servers...");

#ifdef NTP_TIMEZONE
    // Use timezone string (supports automatic DST)
    configTzTime(NTP_TIMEZONE, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
    Serial.printf("[NTP] Timezone: %s\n", NTP_TIMEZONE);
#else
    // Fallback to offset-based config
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
    Serial.printf("[NTP] Offset: GMT%+d, DST: %d\n", GMT_OFFSET_SEC/3600, DAYLIGHT_OFFSET_SEC/3600);
#endif

    Serial.println("[NTP] Servers:");
    Serial.printf("  1. %s (primary)\n", NTP_SERVER_1);
    Serial.printf("  2. %s (fallback)\n", NTP_SERVER_2);
    Serial.printf("  3. %s (fallback)\n", NTP_SERVER_3);
}

// ============================================================================
// Sync NTP Time (blocking, with retries)
// ============================================================================

bool syncNTP() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[NTP] Cannot sync - WiFi not connected");
        ntpStatus = NTP_STATUS_SYNC_FAILED;
        return false;
    }

    ntpStatus = NTP_STATUS_SYNCING;
    lastSyncAttempt = millis();

    Serial.println("[NTP] Starting time synchronization...");

    // Initialize NTP if not done
    static bool initialized = false;
    if (!initialized) {
        initNTP();
        initialized = true;
    }

    // Wait for time to be set
    struct tm timeinfo;
    int attempts = 0;
    int maxAttempts = NTP_SYNC_TIMEOUT_MS / 500;

    while (!getLocalTime(&timeinfo, 500) && attempts < maxAttempts) {
        attempts++;
        if (attempts % 4 == 0) {
            Serial.printf("[NTP] Waiting for sync... (%d/%d)\n", attempts, maxAttempts);
        }
    }

    if (attempts >= maxAttempts) {
        syncFailCount++;
        ntpStatus = NTP_STATUS_SYNC_FAILED;
        Serial.printf("[NTP] Sync FAILED after %d attempts (total failures: %d)\n",
            attempts, syncFailCount);
        return false;
    }

    // Success
    ntpStatus = NTP_STATUS_SYNCED;
    lastSyncTime = millis();
    syncFailCount = 0;

    char timeStr[64];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.printf("[NTP] Time synced: %s\n", timeStr);
    Serial.printf("[NTP] Timezone: %s\n", getenv("TZ") ? getenv("TZ") : "default");

    return true;
}

// ============================================================================
// Check if Resync Needed (call periodically in loop)
// ============================================================================

bool checkNTPResync() {
    // Don't resync if WiFi is down
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    uint32_t now = millis();

    // Check if it's time for periodic resync
    if (ntpStatus == NTP_STATUS_SYNCED) {
        if (now - lastSyncTime >= NTP_RESYNC_INTERVAL_MS) {
            Serial.println("[NTP] Periodic resync...");
            return syncNTP();
        }
    }

    // If sync failed, retry with backoff
    if (ntpStatus == NTP_STATUS_SYNC_FAILED || ntpStatus == NTP_STATUS_NOT_SYNCED) {
        uint32_t backoff = min(60000UL * (1 << min(syncFailCount, 5)), 3600000UL);
        if (now - lastSyncAttempt >= backoff) {
            Serial.printf("[NTP] Retry sync (backoff: %lu ms)\n", backoff);
            return syncNTP();
        }
    }

    return false;
}

// ============================================================================
// Get Current Time
// ============================================================================

bool getCurrentTime(struct tm *t) {
    return getLocalTime(t, 10);  // 10ms timeout
}

time_t getCurrentTimestamp() {
    time_t now;
    time(&now);
    return now;
}

// ============================================================================
// Format Functions
// ============================================================================

void formatTime24h(char *buffer, size_t size) {
    struct tm t;
    if (getCurrentTime(&t)) {
        snprintf(buffer, size, "%02d:%02d", t.tm_hour, t.tm_min);
    } else {
        snprintf(buffer, size, "--:--");
    }
}

void formatTimeWithSeconds(char *buffer, size_t size) {
    struct tm t;
    if (getCurrentTime(&t)) {
        snprintf(buffer, size, "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
    } else {
        snprintf(buffer, size, "--:--:--");
    }
}

void formatDateShort(char *buffer, size_t size) {
    struct tm t;
    if (getCurrentTime(&t)) {
        snprintf(buffer, size, "%02d/%02d/%04d",
            t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
    } else {
        snprintf(buffer, size, "--/--/----");
    }
}

void formatDateLong(char *buffer, size_t size) {
    static const char *days[] = {
        "Domingo", "Lunes", "Martes", "Miercoles",
        "Jueves", "Viernes", "Sabado"
    };
    static const char *months[] = {
        "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
        "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
    };

    struct tm t;
    if (getCurrentTime(&t)) {
        snprintf(buffer, size, "%s, %d de %s",
            days[t.tm_wday], t.tm_mday, months[t.tm_mon]);
    } else {
        snprintf(buffer, size, "---");
    }
}

void formatDateISO(char *buffer, size_t size) {
    struct tm t;
    if (getCurrentTime(&t)) {
        strftime(buffer, size, "%Y-%m-%d", &t);
    } else {
        snprintf(buffer, size, "----/--/--");
    }
}

// ============================================================================
// Day/Month Names
// ============================================================================

const char* getDayName(int wday, bool abbreviated) {
    static const char *days_full[] = {
        "Domingo", "Lunes", "Martes", "Miercoles",
        "Jueves", "Viernes", "Sabado"
    };
    static const char *days_short[] = {
        "Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab"
    };

    if (wday < 0 || wday > 6) return "";
    return abbreviated ? days_short[wday] : days_full[wday];
}

const char* getMonthName(int month, bool abbreviated) {
    static const char *months_full[] = {
        "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
        "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
    };
    static const char *months_short[] = {
        "Ene", "Feb", "Mar", "Abr", "May", "Jun",
        "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"
    };

    if (month < 0 || month > 11) return "";
    return abbreviated ? months_short[month] : months_full[month];
}

// ============================================================================
// Time Query Functions
// ============================================================================

int getCurrentHour() {
    struct tm t;
    if (getCurrentTime(&t)) {
        return t.tm_hour;
    }
    return -1;
}

int getCurrentMinute() {
    struct tm t;
    if (getCurrentTime(&t)) {
        return t.tm_min;
    }
    return -1;
}

int getCurrentSecond() {
    struct tm t;
    if (getCurrentTime(&t)) {
        return t.tm_sec;
    }
    return -1;
}

int getCurrentDayOfWeek() {
    struct tm t;
    if (getCurrentTime(&t)) {
        return t.tm_wday;  // 0 = Sunday
    }
    return -1;
}

int getCurrentDayOfMonth() {
    struct tm t;
    if (getCurrentTime(&t)) {
        return t.tm_mday;
    }
    return -1;
}

int getCurrentMonth() {
    struct tm t;
    if (getCurrentTime(&t)) {
        return t.tm_mon;  // 0 = January
    }
    return -1;
}

int getCurrentYear() {
    struct tm t;
    if (getCurrentTime(&t)) {
        return t.tm_year + 1900;
    }
    return -1;
}

// ============================================================================
// Status Functions
// ============================================================================

bool isTimeValid() {
    struct tm t;
    if (!getCurrentTime(&t)) return false;
    return t.tm_year > 100;  // Year > 2000
}

bool isTimeSynced() {
    return ntpStatus == NTP_STATUS_SYNCED && isTimeValid();
}

NTPStatus getNTPStatus() {
    return ntpStatus;
}

const char* getNTPStatusText() {
    switch (ntpStatus) {
        case NTP_STATUS_NOT_SYNCED: return "NO SINCRONIZADO";
        case NTP_STATUS_SYNCING:    return "SINCRONIZANDO...";
        case NTP_STATUS_SYNCED:     return "SINCRONIZADO";
        case NTP_STATUS_SYNC_FAILED: return "ERROR SYNC";
        default: return "DESCONOCIDO";
    }
}

uint32_t getSecondsSinceSync() {
    if (lastSyncTime == 0) return UINT32_MAX;
    return (millis() - lastSyncTime) / 1000;
}

uint32_t getMinutesSinceSync() {
    return getSecondsSinceSync() / 60;
}

// ============================================================================
// Time Period Helpers
// ============================================================================

bool isDaytime() {
    int hour = getCurrentHour();
    if (hour < 0) return true;  // Default to day if unknown
    return hour >= 6 && hour < 20;  // 6:00 - 19:59
}

bool isNighttime() {
    return !isDaytime();
}

bool isMorning() {
    int hour = getCurrentHour();
    return hour >= 6 && hour < 12;
}

bool isAfternoon() {
    int hour = getCurrentHour();
    return hour >= 12 && hour < 18;
}

bool isEvening() {
    int hour = getCurrentHour();
    return hour >= 18 && hour < 22;
}

// ============================================================================
// Epoch Time Helpers
// ============================================================================

time_t getStartOfDay(time_t timestamp) {
    struct tm t;
    localtime_r(&timestamp, &t);
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    return mktime(&t);
}

time_t getStartOfToday() {
    return getStartOfDay(getCurrentTimestamp());
}

int getDaysBetween(time_t t1, time_t t2) {
    return (getStartOfDay(t2) - getStartOfDay(t1)) / 86400;
}

#endif // NTP_TIME_H
