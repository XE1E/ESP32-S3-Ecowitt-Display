/**
 * history_storage.h - Weather History Storage
 *
 * Stores weather data in memory (ring buffer) and SD card
 * - 3 days detailed history (432 points at 10-min intervals)
 * - 7 days extended history (1008 points)
 */

#ifndef HISTORY_STORAGE_H
#define HISTORY_STORAGE_H

#include <Arduino.h>
#include "config.h"

#ifdef USE_SD_CARD
#include <SPI.h>
#include <SD.h>
#endif

// ============================================================================
// Ring Buffer for Memory Storage
// ============================================================================

class HistoryBuffer {
private:
    HistoryPoint *buffer;
    int capacity;
    int head;
    int count;

public:
    HistoryBuffer(int size) : capacity(size), head(0), count(0) {
        buffer = (HistoryPoint *)ps_malloc(size * sizeof(HistoryPoint));
        if (!buffer) {
            Serial.println("ERROR: Failed to allocate history buffer in PSRAM");
        }
    }

    ~HistoryBuffer() {
        if (buffer) free(buffer);
    }

    bool isValid() { return buffer != nullptr; }

    void add(const HistoryPoint &point) {
        if (!buffer) return;
        buffer[head] = point;
        head = (head + 1) % capacity;
        if (count < capacity) count++;
    }

    int getCount() { return count; }

    // Get point by index (0 = oldest, count-1 = newest)
    bool get(int index, HistoryPoint *point) {
        if (!buffer || index < 0 || index >= count) return false;
        int actualIndex = (head - count + index + capacity) % capacity;
        *point = buffer[actualIndex];
        return true;
    }

    // Get points from last N hours
    int getLastHours(int hours, HistoryPoint *output, int maxOutput) {
        if (!buffer) return 0;

        uint32_t now = time(nullptr);
        uint32_t cutoff = now - (hours * 3600);
        int outputCount = 0;

        for (int i = 0; i < count && outputCount < maxOutput; i++) {
            HistoryPoint p;
            if (get(i, &p) && p.timestamp >= cutoff) {
                output[outputCount++] = p;
            }
        }
        return outputCount;
    }

    // Calculate statistics for a time range
    HistoryStats getStats(int hours) {
        HistoryStats stats = {0};
        if (!buffer || count == 0) return stats;

        uint32_t now = time(nullptr);
        uint32_t cutoff = now - (hours * 3600);

        float tempSum = 0;
        bool first = true;

        for (int i = 0; i < count; i++) {
            HistoryPoint p;
            if (!get(i, &p) || p.timestamp < cutoff) continue;

            if (first) {
                stats.temp_min = stats.temp_max = p.temperature;
                stats.pressure_min = stats.pressure_max = p.pressure;
                stats.humidity_min = stats.humidity_max = p.humidity;
                first = false;
            } else {
                if (p.temperature < stats.temp_min) stats.temp_min = p.temperature;
                if (p.temperature > stats.temp_max) stats.temp_max = p.temperature;
                if (p.pressure < stats.pressure_min) stats.pressure_min = p.pressure;
                if (p.pressure > stats.pressure_max) stats.pressure_max = p.pressure;
                if (p.humidity < stats.humidity_min) stats.humidity_min = p.humidity;
                if (p.humidity > stats.humidity_max) stats.humidity_max = p.humidity;
            }

            tempSum += p.temperature;
            stats.total_precipitation += p.precipitation;
            stats.data_points++;
        }

        if (stats.data_points > 0) {
            stats.temp_avg = tempSum / stats.data_points;
        }

        return stats;
    }

    void clear() {
        head = 0;
        count = 0;
    }
};

// ============================================================================
// Global History Buffer
// ============================================================================

static HistoryBuffer *historyBuffer = nullptr;

// ============================================================================
// SD Card Functions
// ============================================================================

#ifdef USE_SD_CARD

static bool sdCardInitialized = false;
static SPIClass sdSPI(HSPI);

bool initSDCard() {
    Serial.println("Initializing SD card...");

    sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

    if (!SD.begin(SD_CS, sdSPI)) {
        Serial.println("SD card initialization failed!");
        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return false;
    }

    Serial.printf("SD Card Type: %s\n",
        cardType == CARD_MMC ? "MMC" :
        cardType == CARD_SD ? "SDSC" :
        cardType == CARD_SDHC ? "SDHC" : "UNKNOWN");

    Serial.printf("SD Card Size: %lluMB\n", SD.cardSize() / (1024 * 1024));

    // Create history directory if it doesn't exist
    if (!SD.exists("/history")) {
        SD.mkdir("/history");
    }

    sdCardInitialized = true;
    return true;
}

bool isSDCardReady() {
    return sdCardInitialized;
}

// Save a single point to daily CSV file
bool savePointToSD(const HistoryPoint &point) {
    if (!sdCardInitialized) return false;

    // Create filename based on date: /history/YYYYMMDD.csv
    struct tm timeinfo;
    time_t t = point.timestamp;
    localtime_r(&t, &timeinfo);

    char filename[32];
    snprintf(filename, sizeof(filename), "/history/%04d%02d%02d.csv",
        timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

    bool fileExists = SD.exists(filename);
    File file = SD.open(filename, FILE_APPEND);

    if (!file) {
        Serial.printf("Failed to open %s\n", filename);
        return false;
    }

    // Write header if new file
    if (!fileExists) {
        file.println("timestamp,temperature,pressure,humidity,precipitation");
    }

    // Write data
    file.printf("%lu,%.1f,%.1f,%d,%.1f\n",
        point.timestamp, point.temperature, point.pressure,
        point.humidity, point.precipitation);

    file.close();
    return true;
}

// Load history from SD card (last N days)
int loadHistoryFromSD(int days, HistoryPoint *output, int maxOutput) {
    if (!sdCardInitialized) return 0;

    int outputCount = 0;
    time_t now = time(nullptr);
    struct tm timeinfo;

    for (int d = days - 1; d >= 0 && outputCount < maxOutput; d--) {
        time_t dayTime = now - (d * 86400);
        localtime_r(&dayTime, &timeinfo);

        char filename[32];
        snprintf(filename, sizeof(filename), "/history/%04d%02d%02d.csv",
            timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

        if (!SD.exists(filename)) continue;

        File file = SD.open(filename, FILE_READ);
        if (!file) continue;

        // Skip header
        file.readStringUntil('\n');

        // Read data lines
        while (file.available() && outputCount < maxOutput) {
            String line = file.readStringUntil('\n');
            if (line.length() < 10) continue;

            HistoryPoint p;
            int parsed = sscanf(line.c_str(), "%lu,%f,%f,%d,%f",
                &p.timestamp, &p.temperature, &p.pressure,
                &p.humidity, &p.precipitation);

            if (parsed == 5) {
                output[outputCount++] = p;
            }
        }

        file.close();
    }

    return outputCount;
}

// Get list of available history files
int getHistoryFileList(char files[][16], int maxFiles) {
    if (!sdCardInitialized) return 0;

    File root = SD.open("/history");
    if (!root || !root.isDirectory()) return 0;

    int count = 0;
    File file = root.openNextFile();

    while (file && count < maxFiles) {
        if (!file.isDirectory()) {
            strlcpy(files[count], file.name(), 16);
            count++;
        }
        file = root.openNextFile();
    }

    return count;
}

// Delete old history files (keep last N days)
void cleanupOldHistory(int keepDays) {
    if (!sdCardInitialized) return;

    time_t now = time(nullptr);
    time_t cutoff = now - (keepDays * 86400);
    struct tm cutoffTm;
    localtime_r(&cutoff, &cutoffTm);

    int cutoffDate = (cutoffTm.tm_year + 1900) * 10000 +
                     (cutoffTm.tm_mon + 1) * 100 +
                     cutoffTm.tm_mday;

    File root = SD.open("/history");
    if (!root || !root.isDirectory()) return;

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            // Parse date from filename (YYYYMMDD.csv)
            int fileDate = atoi(file.name());
            if (fileDate > 0 && fileDate < cutoffDate) {
                char path[32];
                snprintf(path, sizeof(path), "/history/%s", file.name());
                Serial.printf("Deleting old history: %s\n", path);
                SD.remove(path);
            }
        }
        file = root.openNextFile();
    }
}

#endif // USE_SD_CARD

// ============================================================================
// High-level History Functions
// ============================================================================

void initHistory() {
    Serial.println("Initializing history storage...");

    // Allocate memory buffer
    historyBuffer = new HistoryBuffer(HISTORY_MAX_POINTS);

    if (historyBuffer && historyBuffer->isValid()) {
        Serial.printf("History buffer allocated: %d points\n", HISTORY_MAX_POINTS);
    }

    #ifdef USE_SD_CARD
    initSDCard();

    // Load recent history from SD card into memory
    if (sdCardInitialized && historyBuffer) {
        HistoryPoint *tempBuffer = (HistoryPoint *)ps_malloc(HISTORY_MAX_POINTS * sizeof(HistoryPoint));
        if (tempBuffer) {
            int loaded = loadHistoryFromSD(7, tempBuffer, HISTORY_MAX_POINTS);
            for (int i = 0; i < loaded; i++) {
                historyBuffer->add(tempBuffer[i]);
            }
            free(tempBuffer);
            Serial.printf("Loaded %d history points from SD\n", loaded);
        }
    }
    #endif
}

void recordHistoryPoint(float temperature, float pressure, int humidity, float precipitation) {
    if (!historyBuffer) return;

    HistoryPoint point;
    point.timestamp = time(nullptr);
    point.temperature = temperature;
    point.pressure = pressure;
    point.humidity = humidity;
    point.precipitation = precipitation;

    // Add to memory buffer
    historyBuffer->add(point);

    #ifdef USE_SD_CARD
    // Save to SD card
    savePointToSD(point);
    #endif

    Serial.printf("History recorded: %.1fC, %.1fhPa, %d%%, %.1fmm\n",
        temperature, pressure, humidity, precipitation);
}

HistoryStats getHistory3DayStats() {
    if (!historyBuffer) return HistoryStats{0};
    return historyBuffer->getStats(72);  // 72 hours = 3 days
}

HistoryStats getHistory7DayStats() {
    if (!historyBuffer) return HistoryStats{0};
    return historyBuffer->getStats(168); // 168 hours = 7 days
}

int getHistoryPoints(int hours, HistoryPoint *output, int maxOutput) {
    if (!historyBuffer) return 0;
    return historyBuffer->getLastHours(hours, output, maxOutput);
}

int getHistoryCount() {
    if (!historyBuffer) return 0;
    return historyBuffer->getCount();
}

#endif // HISTORY_STORAGE_H
