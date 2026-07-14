/**
 * data_watchdog.h - API Data Watchdog
 *
 * Monitors API connection status and triggers UI updates
 * when data becomes stale or API is unreachable.
 */

#ifndef DATA_WATCHDOG_H
#define DATA_WATCHDOG_H

#include <Arduino.h>

// ============================================================================
// Configuration
// ============================================================================

#define WATCHDOG_MAX_FAILURES   3       // Max consecutive failures before "disconnected"
#define WATCHDOG_STALE_MINUTES  30      // Data considered stale after N minutes

// ============================================================================
// Connection Status Enum
// ============================================================================

enum DataStatus {
    DATA_STATUS_OK = 0,         // Data fresh and valid
    DATA_STATUS_STALE,          // Data older than threshold
    DATA_STATUS_DISCONNECTED,   // API unreachable (3+ failures)
    DATA_STATUS_NO_WIFI         // WiFi not connected
};

// ============================================================================
// Watchdog Class
// ============================================================================

class DataWatchdog {
private:
    int failureCount;
    uint32_t lastSuccessTime;
    uint32_t lastAttemptTime;
    DataStatus currentStatus;
    bool statusChanged;

public:
    DataWatchdog()
        : failureCount(0),
          lastSuccessTime(0),
          lastAttemptTime(0),
          currentStatus(DATA_STATUS_DISCONNECTED),
          statusChanged(false) {}

    // Call when API request succeeds
    void onSuccess() {
        DataStatus oldStatus = currentStatus;
        failureCount = 0;
        lastSuccessTime = millis();
        lastAttemptTime = lastSuccessTime;
        currentStatus = DATA_STATUS_OK;
        statusChanged = (oldStatus != currentStatus);

        if (statusChanged) {
            Serial.println("[Watchdog] Status: OK - Data updated successfully");
        }
    }

    // Call when API request fails
    void onFailure() {
        DataStatus oldStatus = currentStatus;
        failureCount++;
        lastAttemptTime = millis();

        if (failureCount >= WATCHDOG_MAX_FAILURES) {
            currentStatus = DATA_STATUS_DISCONNECTED;
        }

        statusChanged = (oldStatus != currentStatus);

        Serial.printf("[Watchdog] Failure %d/%d\n", failureCount, WATCHDOG_MAX_FAILURES);

        if (statusChanged && currentStatus == DATA_STATUS_DISCONNECTED) {
            Serial.println("[Watchdog] Status: DISCONNECTED - API unreachable");
        }
    }

    // Call when WiFi disconnects
    void onWiFiDisconnected() {
        DataStatus oldStatus = currentStatus;
        currentStatus = DATA_STATUS_NO_WIFI;
        statusChanged = (oldStatus != currentStatus);

        if (statusChanged) {
            Serial.println("[Watchdog] Status: NO_WIFI");
        }
    }

    // Call periodically to check for stale data
    void checkStaleData() {
        if (currentStatus == DATA_STATUS_OK && lastSuccessTime > 0) {
            uint32_t elapsed = (millis() - lastSuccessTime) / 1000 / 60;  // minutes

            if (elapsed >= WATCHDOG_STALE_MINUTES) {
                DataStatus oldStatus = currentStatus;
                currentStatus = DATA_STATUS_STALE;
                statusChanged = (oldStatus != currentStatus);

                if (statusChanged) {
                    Serial.printf("[Watchdog] Status: STALE - Data is %d min old\n", elapsed);
                }
            }
        }
    }

    // Get current status
    DataStatus getStatus() const { return currentStatus; }

    // Check if status changed since last check
    bool hasStatusChanged() {
        bool changed = statusChanged;
        statusChanged = false;  // Reset flag
        return changed;
    }

    // Get failure count
    int getFailureCount() const { return failureCount; }

    // Get minutes since last successful update
    uint32_t getMinutesSinceUpdate() const {
        if (lastSuccessTime == 0) return 9999;
        return (millis() - lastSuccessTime) / 1000 / 60;
    }

    // Get status as string
    const char* getStatusText() const {
        switch (currentStatus) {
            case DATA_STATUS_OK:           return "CONECTADO";
            case DATA_STATUS_STALE:        return "DATOS ANTIGUOS";
            case DATA_STATUS_DISCONNECTED: return "SIN CONEXION";
            case DATA_STATUS_NO_WIFI:      return "SIN WIFI";
            default:                       return "DESCONOCIDO";
        }
    }

    // Check if data is usable (OK or Stale)
    bool isDataUsable() const {
        return currentStatus == DATA_STATUS_OK || currentStatus == DATA_STATUS_STALE;
    }

    // Reset watchdog
    void reset() {
        failureCount = 0;
        lastSuccessTime = 0;
        lastAttemptTime = 0;
        currentStatus = DATA_STATUS_DISCONNECTED;
        statusChanged = true;
    }
};

// ============================================================================
// Global Instance
// ============================================================================

extern DataWatchdog dataWatchdog;

#endif // DATA_WATCHDOG_H
