/**
 * bme280_sensor.h - BME280 Indoor Sensor
 *
 * Sensor local de temperatura, humedad y presión.
 * Puede enviar datos al servidor como estación remota.
 */

#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "config.h"

// BME280 instance
static Adafruit_BME280 bme;
static bool bmeInitialized = false;

// ============================================================================
// Initialize BME280
// ============================================================================

bool initBME280(uint8_t address = 0x76) {
    Serial.println("[BME280] Inicializando...");

    // I2C ya inicializado por el touch (GPIO 19/20)
    if (!bme.begin(address)) {
        Serial.println("[BME280] No encontrado!");
        bmeInitialized = false;
        return false;
    }

    // Configurar para monitoreo interior
    bme.setSampling(
        Adafruit_BME280::MODE_NORMAL,
        Adafruit_BME280::SAMPLING_X2,   // temperature
        Adafruit_BME280::SAMPLING_X16,  // pressure
        Adafruit_BME280::SAMPLING_X1,   // humidity
        Adafruit_BME280::FILTER_X16,
        Adafruit_BME280::STANDBY_MS_500
    );

    bmeInitialized = true;
    Serial.println("[BME280] Inicializado OK");
    return true;
}

// ============================================================================
// Read Sensor Data
// ============================================================================

bool readBME280(LocalSensorData& data) {
    if (!bmeInitialized) {
        data.valid = false;
        return false;
    }

    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.pressure = bme.readPressure() / 100.0F;  // Convertir a hPa
    data.valid = true;
    data.last_read = millis();

    return true;
}

// ============================================================================
// Get Sensor Status
// ============================================================================

bool isBME280Available() {
    return bmeInitialized;
}

// ============================================================================
// Get formatted string
// ============================================================================

void getBME280String(char* buffer, size_t size, const LocalSensorData& data) {
    if (data.valid) {
        snprintf(buffer, size, "%.1f°C  %.0f%%  %.0fhPa",
                 data.temperature, data.humidity, data.pressure);
    } else {
        snprintf(buffer, size, "-- (sensor no disponible)");
    }
}

#endif // BME280_SENSOR_H
