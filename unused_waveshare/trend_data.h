/**
 * trend_data.h - Historical Data for Trend Calculation
 *
 * Uses circular buffers for memory-safe storage.
 * No dynamic allocation = No memory leaks.
 */

#ifndef TREND_DATA_H
#define TREND_DATA_H

#include <Arduino.h>
#include "circular_buffer.h"

// ============================================================================
// Configuration
// ============================================================================

#define TREND_HISTORY_MINUTES   180     // 3 hours
#define TREND_SAMPLE_INTERVAL   600     // 10 minutes in seconds

// Thresholds for trend detection
#define TREND_TEMP_THRESHOLD    0.3f    // °C change to consider significant
#define TREND_PRESSURE_THRESHOLD 0.5f   // mb change to consider significant
#define TREND_HUMIDITY_THRESHOLD 2.0f   // % change to consider significant

// ============================================================================
// Trend Direction Enum
// ============================================================================

enum TrendDirection {
    TREND_STABLE = 0,   // No significant change (→)
    TREND_RISING = 1,   // Increasing (▲)
    TREND_FALLING = -1  // Decreasing (▼)
};

// ============================================================================
// Trend Result Structure
// ============================================================================

struct TrendResult {
    TrendDirection direction;
    float change;           // Absolute change over period
    float changePerHour;    // Rate of change per hour
};

// ============================================================================
// Trend Manager Class
// ============================================================================

class TrendManager {
private:
    // Separate buffers for each sensor (3 hours @ 10 min = 18 samples)
    SensorHistory<HISTORY_3H_SAMPLES> tempHistory;
    SensorHistory<HISTORY_3H_SAMPLES> pressureHistory;
    SensorHistory<HISTORY_3H_SAMPLES> humidityHistory;

    TrendDirection calculateDirection(float change, float threshold) {
        if (change > threshold) return TREND_RISING;
        if (change < -threshold) return TREND_FALLING;
        return TREND_STABLE;
    }

public:
    TrendManager()
        : tempHistory(TREND_SAMPLE_INTERVAL),
          pressureHistory(TREND_SAMPLE_INTERVAL),
          humidityHistory(TREND_SAMPLE_INTERVAL) {}

    // Add new samples (call when weather data updates)
    void addSamples(float temp, float pressure, int humidity) {
        bool added = false;

        if (tempHistory.addSample(temp)) added = true;
        if (pressureHistory.addSample(pressure)) added = true;
        if (humidityHistory.addSample((float)humidity)) added = true;

        if (added) {
            Serial.printf("[Trend] Sample added: T=%.1f P=%.1f H=%d (count=%d)\n",
                temp, pressure, humidity, tempHistory.size());
        }
    }

    // Get temperature trend
    TrendResult getTemperatureTrend() {
        TrendResult result;
        result.change = tempHistory.getChange();
        result.changePerHour = tempHistory.getChangePerHour();
        result.direction = calculateDirection(result.change, TREND_TEMP_THRESHOLD);
        return result;
    }

    // Get pressure trend
    TrendResult getPressureTrend() {
        TrendResult result;
        result.change = pressureHistory.getChange();
        result.changePerHour = pressureHistory.getChangePerHour();
        result.direction = calculateDirection(result.change, TREND_PRESSURE_THRESHOLD);
        return result;
    }

    // Get humidity trend
    TrendResult getHumidityTrend() {
        TrendResult result;
        result.change = humidityHistory.getChange();
        result.changePerHour = humidityHistory.getChangePerHour();
        result.direction = calculateDirection(result.change, TREND_HUMIDITY_THRESHOLD);
        return result;
    }

    // Check if we have enough data (at least 3 samples = 30 min)
    bool hasEnoughData() {
        return tempHistory.size() >= 3;
    }

    // Get sample count
    size_t getSampleCount() {
        return tempHistory.size();
    }

    // Clear all history
    void clear() {
        tempHistory.clear();
        pressureHistory.clear();
        humidityHistory.clear();
    }
};

// ============================================================================
// Global Instance (extern declaration)
// ============================================================================

extern TrendManager trendManager;

// ============================================================================
// Helper Functions
// ============================================================================

// Get arrow symbol for trend direction (Unicode)
inline const char* getTrendArrow(TrendDirection dir) {
    switch (dir) {
        case TREND_RISING:  return "▲";
        case TREND_FALLING: return "▼";
        default:            return "→";
    }
}

// Get arrow symbol (Unicode - works with all fonts)
inline const char* getTrendArrowLVGL(TrendDirection dir) {
    switch (dir) {
        case TREND_RISING:  return "\xE2\x96\xB2";  // ▲
        case TREND_FALLING: return "\xE2\x96\xBC";  // ▼
        default:            return "\xE2\x96\xB6";  // ▶
    }
}

// Get color for trend (hex)
inline uint32_t getTrendColor(TrendDirection dir) {
    switch (dir) {
        case TREND_RISING:  return 0xC62828;  // Red
        case TREND_FALLING: return 0x2196F3;  // Blue
        default:            return 0x888888;  // Gray
    }
}

// Format trend string: "+0.5°C/3h" or "-1.2 mb/3h"
inline void formatTrendString(char* buf, size_t size, const TrendResult& result, const char* unit) {
    const char* sign = (result.change >= 0) ? "+" : "";
    snprintf(buf, size, "%s%.1f %s/3h", sign, result.change, unit);
}

// ============================================================================
// Convenience Functions
// ============================================================================

inline void updateTrendHistory(float temp, float pressure, int humidity) {
    trendManager.addSamples(temp, pressure, humidity);
}

inline void getTrendStrings(char* tempStr, char* pressStr, char* humStr, size_t size) {
    if (trendManager.hasEnoughData()) {
        formatTrendString(tempStr, size, trendManager.getTemperatureTrend(), "°C");
        formatTrendString(pressStr, size, trendManager.getPressureTrend(), "mb");
        formatTrendString(humStr, size, trendManager.getHumidityTrend(), "%");
    } else {
        snprintf(tempStr, size, "-- °C/3h");
        snprintf(pressStr, size, "-- mb/3h");
        snprintf(humStr, size, "-- %%/3h");
    }
}

#endif // TREND_DATA_H
