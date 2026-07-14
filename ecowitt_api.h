/**
 * ecowitt_api.h - Cliente REST para servidor Ecowitt
 */

#ifndef ECOWITT_API_H
#define ECOWITT_API_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "my_config.h"

// ============================================================================
// Cliente API
// ============================================================================

class EcowittAPI {
public:
    EcowittAPI() : _baseUrl(API_BASE_URL) {}

    /**
     * Obtiene los datos meteorológicos actuales
     */
    bool fetchCurrent(WeatherData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/current";

        http.begin(url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                // Exterior
                data.temp_outdoor = doc["temperature_outdoor"] | 0.0f;
                data.humidity_outdoor = doc["humidity_outdoor"] | 0.0f;
                data.pressure_rel = doc["pressure_rel"] | 0.0f;
                data.wind_speed = doc["wind_speed"] | 0.0f;
                data.wind_gust = doc["wind_gust"] | 0.0f;
                strlcpy(data.wind_dir_cardinal,
                        doc["wind_dir_cardinal"] | "N",
                        sizeof(data.wind_dir_cardinal));
                data.rain_rate = doc["rain_rate"] | 0.0f;
                data.rain_day = doc["rain_day"] | 0.0f;
                data.uv = doc["uv"] | 0.0f;
                data.solar_radiation = doc["solar_radiation"] | 0.0f;
                data.feels_like = doc["feels_like"] | data.temp_outdoor;
                data.dewpoint = doc["dewpoint"] | 0.0f;

                // Interior
                data.temp_indoor = doc["temperature_indoor"] | 0.0f;
                data.humidity_indoor = doc["humidity_indoor"] | 0.0f;

                // Timestamp
                strlcpy(data.timestamp,
                        doc["timestamp"] | "",
                        sizeof(data.timestamp));

                data.valid = true;
                http.end();
                return true;
            }
        }

        data.valid = false;
        http.end();
        return false;
    }

    /**
     * Obtiene estadísticas del día
     */
    bool fetchDailyStats(WeatherData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/stats/daily";

        http.begin(url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                data.temp_max = doc["temp_max"] | 0.0f;
                data.temp_min = doc["temp_min"] | 0.0f;
                data.wind_max = doc["wind_max"] | 0.0f;
                data.rain_total = doc["rain_total"] | 0.0f;
                http.end();
                return true;
            }
        }

        http.end();
        return false;
    }

    /**
     * Obtiene comparación vs ayer
     */
    bool fetchCompare(CompareData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/compare";

        http.begin(url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                data.temp_diff = doc["temp_diff"] | 0.0f;
                data.humidity_diff = doc["humidity_diff"] | 0.0f;
                data.pressure_diff = doc["pressure_diff"] | 0.0f;
                data.valid = true;
                http.end();
                return true;
            }
        }

        data.valid = false;
        http.end();
        return false;
    }

    /**
     * Obtiene alertas activas
     */
    bool fetchAlerts(AlertData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/alerts";

        http.begin(url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                JsonArray alerts = doc["alerts"].as<JsonArray>();
                data.alert_count = min((int)alerts.size(), 5);
                data.has_alerts = data.alert_count > 0;

                for (int i = 0; i < data.alert_count; i++) {
                    strlcpy(data.alerts[i],
                            alerts[i]["message"] | "",
                            sizeof(data.alerts[i]));
                }

                http.end();
                return true;
            }
        }

        data.has_alerts = false;
        data.alert_count = 0;
        http.end();
        return false;
    }

    /**
     * Obtiene datos del almanaque
     */
    bool fetchAlmanac(AlmanacData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/almanac";

        http.begin(url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                strlcpy(data.sunrise,
                        doc["sun"]["sunrise"] | "00:00",
                        sizeof(data.sunrise));
                strlcpy(data.sunset,
                        doc["sun"]["sunset"] | "00:00",
                        sizeof(data.sunset));
                strlcpy(data.moon_phase,
                        doc["moon"]["phase"] | "",
                        sizeof(data.moon_phase));
                data.moon_illumination = doc["moon"]["illumination"] | 0;
                data.valid = true;
                http.end();
                return true;
            }
        }

        data.valid = false;
        http.end();
        return false;
    }

    /**
     * Verifica conectividad con el servidor
     */
    bool checkConnection() {
        HTTPClient http;
        String url = String(_baseUrl) + "/health";

        http.begin(url);
        http.setTimeout(5000);
        int httpCode = http.GET();
        http.end();

        return httpCode == HTTP_CODE_OK;
    }

    /**
     * Envía datos del BME280 local como estación remota
     * Emula el protocolo Ecowitt para que el servidor lo registre
     * como una estación secundaria (similar a WN31 o GW1100)
     *
     * @param passkey Identificador único de esta "estación"
     * @param temp Temperatura en °C
     * @param humidity Humedad en %
     * @param pressure Presión en hPa
     * @return true si el envío fue exitoso
     */
    bool postLocalSensorData(const char* passkey, float temp, float humidity, float pressure) {
        HTTPClient http;
        String url = String(_baseUrl) + "/data/report/";

        http.begin(url);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.setTimeout(10000);

        // Construir payload en formato Ecowitt
        // El servidor espera unidades imperiales (°F, inHg)
        float temp_f = temp * 9.0 / 5.0 + 32.0;
        float pressure_inhg = pressure * 0.02953;

        String payload = "PASSKEY=" + String(passkey);
        payload += "&stationtype=EcowittDisplay";
        payload += "&dateutc=now";
        payload += "&tempinf=" + String(temp_f, 1);
        payload += "&humidityin=" + String((int)humidity);
        payload += "&baromrelin=" + String(pressure_inhg, 3);
        payload += "&baromabsin=" + String(pressure_inhg, 3);

        int httpCode = http.POST(payload);
        http.end();

        if (httpCode == HTTP_CODE_OK) {
            Serial.printf("[API] Datos locales enviados OK (passkey=%s)\n", passkey);
            return true;
        } else {
            Serial.printf("[API] Error enviando datos locales: %d\n", httpCode);
            return false;
        }
    }

private:
    const char* _baseUrl;
};

// Instancia global
extern EcowittAPI ecowittApi;

#endif // ECOWITT_API_H
