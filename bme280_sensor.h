/**
 * bme280_sensor.h - BME280 Indoor Sensor
 *
 * Optional indoor temperature, humidity, and pressure sensor
 */

#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#ifdef USE_BME280

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "config.h"

// BME280 instance
static Adafruit_BME280 bme;
static bool bmeInitialized = false;
static unsigned long lastSensorRead = 0;

// ============================================================================
// Initialize BME280
// ============================================================================

bool initBME280() {
    Serial.println("Initializing BME280...");

    // Note: I2C should already be initialized by touch

    if (!bme.begin(BME280_ADDR)) {
        Serial.println("BME280 not found!");
        bmeInitialized = false;
        return false;
    }

    // Configure for indoor monitoring
    bme.setSampling(
        Adafruit_BME280::MODE_NORMAL,
        Adafruit_BME280::SAMPLING_X2,   // temperature
        Adafruit_BME280::SAMPLING_X16,  // pressure
        Adafruit_BME280::SAMPLING_X1,   // humidity
        Adafruit_BME280::FILTER_X16,
        Adafruit_BME280::STANDBY_MS_500
    );

    bmeInitialized = true;
    Serial.println("BME280 initialized");
    return true;
}

// ============================================================================
// Read Sensor Data
// ============================================================================

bool readBME280(IndoorData *data) {
    if (!bmeInitialized) {
        data->valid = false;
        return false;
    }

    data->temperature = bme.readTemperature();
    data->humidity = bme.readHumidity();
    data->pressure = bme.readPressure() / 100.0F;  // Convert to hPa
    data->valid = true;

    return true;
}

// ============================================================================
// Update Indoor Sensor (called from loop)
// ============================================================================

void updateIndoorSensor() {
    if (!bmeInitialized) return;

    if (millis() - lastSensorRead < SENSOR_UPDATE_INTERVAL) return;

    lastSensorRead = millis();

    extern IndoorData indoorData;
    if (readBME280(&indoorData)) {
        // Update UI with new readings
        updateIndoorUI(indoorData.temperature, indoorData.humidity);

        Serial.printf("Indoor: %.1f C, %.0f%% RH, %.0f hPa\n",
            indoorData.temperature, indoorData.humidity, indoorData.pressure);
    }
}

// ============================================================================
// Get Sensor Status
// ============================================================================

bool isBME280Available() {
    return bmeInitialized;
}

#endif // USE_BME280

#endif // BME280_SENSOR_H
