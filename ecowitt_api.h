/**
 * ecowitt_api.h - Cliente REST para servidor Ecowitt
 */

#ifndef ECOWITT_API_H
#define ECOWITT_API_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "my_config.h"

// Cliente HTTPS global (reutilizable)
static WiFiClientSecure _secureClient;

// ============================================================================
// Cliente API
// ============================================================================

class EcowittAPI {
public:
    EcowittAPI() : _baseUrl(API_BASE_URL) {
        // Configurar cliente HTTPS sin verificación de certificado
        _secureClient.setInsecure();
    }

    /**
     * Inicia conexión HTTP/HTTPS según la URL
     */
    bool beginRequest(HTTPClient& http, const String& url) {
        if (url.startsWith("https://")) {
            return http.begin(_secureClient, url);
        } else {
            return beginRequest(http, url);
        }
    }

    /**
     * Obtiene los datos meteorológicos actuales
     */
    bool fetchCurrent(WeatherData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/current";

        if (!beginRequest(http, url)) {
            Serial.println("[API] Error iniciando conexion");
            return false;
        }
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                // Exterior (WS69)
                data.temp_outdoor = doc["temperature_outdoor"] | 0.0f;
                data.humidity_outdoor = doc["humidity_outdoor"] | 0.0f;
                data.pressure_rel = doc["pressure_relative"] | 0.0f;
                data.wind_speed = doc["wind_speed"] | 0.0f;
                data.wind_gust = doc["wind_gust"] | 0.0f;
                data.wind_dir = doc["wind_direction"] | 0.0f;

                // Derivar cardinal de grados
                int dir = (int)data.wind_dir;
                const char* cardinals[] = {"N","NNE","NE","ENE","E","ESE","SE","SSE",
                                           "S","SSO","SO","OSO","O","ONO","NO","NNO"};
                int idx = ((dir + 11) / 22) % 16;
                strlcpy(data.wind_dir_cardinal, cardinals[idx], sizeof(data.wind_dir_cardinal));

                data.rain_rate = doc["rain_rate"] | 0.0f;
                data.rain_day = doc["rain_daily"] | 0.0f;
                data.uv = doc["uv_index"] | 0.0f;
                data.solar_radiation = doc["solar_radiation"] | 0.0f;
                data.feels_like = doc["feels_like"] | data.temp_outdoor;
                data.dewpoint = doc["dew_point"] | 0.0f;

                // Interior (WS2910 consola)
                data.temp_indoor = doc["temperature_indoor"] | 0.0f;
                data.humidity_indoor = doc["humidity_indoor"] | 0.0f;

                // Batería WS69 (sensor exterior) - true=OK, false=baja
                data.battery_wh65 = doc["battery_wh65"] | true;

                // Timestamp
                strlcpy(data.timestamp,
                        doc["received_at"] | "",
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

        beginRequest(http, url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                // Estructura: {"stats": {"temperature_outdoor": {"min": X, "max": Y}}}
                JsonObject stats = doc["stats"];
                data.temp_max = stats["temperature_outdoor"]["max"] | 0.0f;
                data.temp_min = stats["temperature_outdoor"]["min"] | 0.0f;
                data.wind_max = stats["wind_gust"]["max"] | 0.0f;
                data.rain_total = stats["rain_daily"]["max"] | 0.0f;
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

        beginRequest(http, url);
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

        beginRequest(http, url);
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

        beginRequest(http, url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                // API usa "rise"/"set", no "sunrise"/"sunset"
                strlcpy(data.sunrise,
                        doc["sun"]["rise"] | "00:00",
                        sizeof(data.sunrise));
                strlcpy(data.sunset,
                        doc["sun"]["set"] | "00:00",
                        sizeof(data.sunset));
                strlcpy(data.moon_phase,
                        doc["moon"]["phase"] | "",
                        sizeof(data.moon_phase));
                // illumination viene como float (ej: 10.0)
                data.moon_illumination = doc["moon"]["illumination"].as<int>();
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

        beginRequest(http, url);
        http.setTimeout(5000);
        int httpCode = http.GET();
        http.end();

        return httpCode == HTTP_CODE_OK;
    }

    /**
     * Obtiene datos de sensor WN31 (canal específico de la estación principal)
     * @param channel Canal del WN31 (1-8)
     * @param data Estructura para almacenar los datos
     */
    bool fetchWN31Channel(int channel, RemoteSensorData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/current";

        beginRequest(http, url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                // Campos del canal WN31: temperature_ch1, humidity_ch1, battery_ch1, etc.
                char temp_key[20], hum_key[20], bat_key[20];
                snprintf(temp_key, sizeof(temp_key), "temperature_ch%d", channel);
                snprintf(hum_key, sizeof(hum_key), "humidity_ch%d", channel);
                snprintf(bat_key, sizeof(bat_key), "battery_ch%d", channel);

                // Verificar si el canal existe
                if (doc.containsKey(temp_key)) {
                    data.temperature = doc[temp_key] | 0.0f;
                    data.humidity = doc[hum_key] | 0.0f;
                    // battery_chX es booleano: true=OK, false=baja
                    bool bat_ok = doc[bat_key] | true;
                    data.battery = bat_ok ? 100 : 10;  // Mostrar 100% o 10% (baja)
                    data.temp_max = data.temperature;
                    data.temp_min = data.temperature;
                    data.valid = true;
                    data.last_update = millis();
                    http.end();
                    return true;
                }
            }
        }

        data.valid = false;
        http.end();
        return false;
    }

    /**
     * Obtiene datos de sensor remoto genérico (estación secundaria)
     * @param station_id Identificador de la estación (ej: "gw1100")
     * @param data Estructura para almacenar los datos
     */
    bool fetchRemoteSensor(const char* station_id, RemoteSensorData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/current?station=" + station_id;

        beginRequest(http, url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(2048);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                data.temperature = doc["temperature_outdoor"] | doc["temperature"] | 0.0f;
                data.humidity = doc["humidity_outdoor"] | doc["humidity"] | 0.0f;
                data.temp_max = doc["temp_max"] | data.temperature;
                data.temp_min = doc["temp_min"] | data.temperature;
                data.battery = doc["battery"] | 100;
                data.valid = true;
                data.last_update = millis();
                http.end();
                return true;
            }
        }

        data.valid = false;
        http.end();
        return false;
    }

    /**
     * Obtiene datos de gateway remoto (GW1100, GW110, etc.)
     * @param station_id Identificador de la estación
     * @param data Estructura para almacenar los datos
     */
    bool fetchRemoteGateway(const char* station_id, RemoteGatewayData& data) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/current?station=" + station_id;

        beginRequest(http, url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(2048);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                // GW1100 usa _indoor, otros gateways pueden usar _outdoor o sin sufijo
                data.temperature = doc["temperature_outdoor"] | doc["temperature_indoor"] | doc["temperature"] | 0.0f;
                data.humidity = doc["humidity_outdoor"] | doc["humidity_indoor"] | doc["humidity"] | 0.0f;
                data.pressure = doc["pressure_relative"] | doc["pressure"] | 0.0f;
                data.temp_max = doc["temp_max"] | data.temperature;
                data.temp_min = doc["temp_min"] | data.temperature;
                data.valid = true;
                data.last_update = millis();
                http.end();
                return true;
            }
        }

        data.valid = false;
        http.end();
        return false;
    }

    /**
     * Obtiene lista de estaciones disponibles
     */
    bool fetchStations(char stations[][32], int& count, int maxStations = 10) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/stations";

        beginRequest(http, url);
        http.setTimeout(10000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(2048);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                JsonArray arr = doc["stations"].as<JsonArray>();
                count = 0;
                for (JsonVariant v : arr) {
                    if (count >= maxStations) break;
                    strlcpy(stations[count], v["id"] | "", 32);
                    count++;
                }
                http.end();
                return true;
            }
        }

        count = 0;
        http.end();
        return false;
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

        beginRequest(http, url);
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

    /**
     * Endpoint optimizado: obtiene TODOS los datos en una sola llamada
     * Combina: current, stats, compare, almanac, forecast, airquality
     * @param tzOffset Puntero para almacenar el offset de timezone (en horas, ej: -6)
     * @param forecast Puntero a estructura de pronóstico (opcional)
     * @param airquality Puntero a estructura de calidad del aire (opcional)
     * @param stats Puntero a estructura de stats con horas (opcional)
     */
    bool fetchAll(WeatherData& weather, CompareData& compare, AlmanacData& almanac,
                  RemoteSensorData& jardin, RemoteGatewayData& remoto, int* tzOffset = nullptr,
                  ForecastData* forecast = nullptr, AirQualityData* airquality = nullptr,
                  StatsData* stats = nullptr) {
        HTTPClient http;
        String url = String(_baseUrl) + "/api/display";

        if (!beginRequest(http, url)) {
            Serial.println("[API] Error iniciando conexion display");
            return false;
        }
        http.setTimeout(15000);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            DynamicJsonDocument doc(8192);
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                // === TIMEZONE ===
                if (tzOffset != nullptr) {
                    *tzOffset = doc["timezone_offset"] | -6;  // Default UTC-6 (Mexico)
                }

                // === CURRENT ===
                JsonObject current = doc["current"];

                // Solo actualizar si el servidor envía datos válidos del sensor exterior
                // (a veces el servidor solo envía datos de la consola sin el WS69)
                float new_temp = current["temperature_outdoor"] | -999.0f;
                float new_hum = current["humidity_outdoor"] | -999.0f;
                float new_pres = current["pressure_relative"] | -999.0f;

                bool hasOutdoorData = (new_temp > -900 && new_hum > -900 && new_pres > -900);

                if (hasOutdoorData) {
                    // Datos válidos del sensor exterior - actualizar todo
                    weather.temp_outdoor = new_temp;
                    weather.humidity_outdoor = new_hum;
                    weather.pressure_rel = new_pres;

                    weather.wind_speed = current["wind_speed"] | 0.0f;
                    weather.wind_gust = current["wind_gust"] | 0.0f;
                    weather.wind_dir = current["wind_direction"] | 0.0f;

                    int dir = (int)weather.wind_dir;
                    const char* cardinals[] = {"N","NNE","NE","ENE","E","ESE","SE","SSE",
                                               "S","SSO","SO","OSO","O","ONO","NO","NNO"};
                    int idx = ((dir + 11) / 22) % 16;
                    strlcpy(weather.wind_dir_cardinal, cardinals[idx], sizeof(weather.wind_dir_cardinal));

                    weather.rain_rate = current["rain_rate"] | 0.0f;
                    weather.rain_day = current["rain_daily"] | 0.0f;
                    weather.rain_week = current["rain_weekly"] | 0.0f;
                    weather.rain_month = current["rain_monthly"] | 0.0f;
                    weather.rain_year = current["rain_yearly"] | 0.0f;
                    weather.uv = current["uv_index"] | 0.0f;
                    weather.solar_radiation = current["solar_radiation"] | 0.0f;
                    weather.feels_like = current["feels_like"] | weather.temp_outdoor;
                    weather.dewpoint = current["dew_point"] | 0.0f;
                    weather.battery_wh65 = current["battery_wh65"] | true;
                } else {
                    // Servidor envió datos incompletos - mantener valores anteriores
                    Serial.println("[API] WARN: Datos exteriores incompletos del servidor, manteniendo valores anteriores");
                }

                // Datos interiores siempre se actualizan (vienen de la consola)
                weather.temp_indoor = current["temperature_indoor"] | 0.0f;
                weather.humidity_indoor = current["humidity_indoor"] | 0.0f;

                strlcpy(weather.timestamp, current["received_at"] | "", sizeof(weather.timestamp));


                // === STATS (min/max del día) ===
                JsonObject statsObj = doc["stats"];
                weather.temp_max = statsObj["temperature_outdoor"]["max"] | 0.0f;
                weather.temp_min = statsObj["temperature_outdoor"]["min"] | 0.0f;
                weather.wind_max = statsObj["wind_gust"]["max"] | 0.0f;
                weather.rain_total = statsObj["rain_daily"]["max"] | 0.0f;

                weather.valid = true;

                // === COMPARE ===
                JsonObject comp = doc["compare"];
                compare.temp_diff = comp["temp_diff"] | 0.0f;
                compare.humidity_diff = comp["humidity_diff"] | 0.0f;
                compare.pressure_diff = comp["pressure_diff"] | 0.0f;
                compare.valid = true;

                // === ALMANAC ===
                JsonObject alm = doc["almanac"];
                strlcpy(almanac.sunrise, alm["sunrise"] | "00:00", sizeof(almanac.sunrise));
                strlcpy(almanac.sunset, alm["sunset"] | "00:00", sizeof(almanac.sunset));
                strlcpy(almanac.moon_phase, alm["moon_phase"] | "", sizeof(almanac.moon_phase));
                almanac.moon_illumination = alm["moon_illumination"] | 0;
                almanac.valid = true;
                Serial.printf("[API] Almanac - Luna: fase='%s', ilum=%d%%\n",
                              almanac.moon_phase, almanac.moon_illumination);

                // === FORECAST (pronóstico barométrico) ===
                if (forecast != nullptr && doc.containsKey("forecast")) {
                    JsonObject fc = doc["forecast"];
                    forecast->available = fc["available"] | false;
                    if (forecast->available) {
                        strlcpy(forecast->forecast, fc["forecast"] | "", sizeof(forecast->forecast));
                        forecast->pressure = fc["pressure"] | 0.0f;
                        forecast->delta_3h = fc["delta_3h"] | 0.0f;
                        // Trend es un objeto anidado
                        JsonObject trend = fc["trend"];
                        strlcpy(forecast->trend_code, trend["code"] | "", sizeof(forecast->trend_code));
                        strlcpy(forecast->trend_label, trend["label"] | "", sizeof(forecast->trend_label));
                        strlcpy(forecast->trend_arrow, trend["arrow"] | "", sizeof(forecast->trend_arrow));
                        forecast->valid = true;
                    } else {
                        forecast->valid = false;
                    }
                }

                // === AIRQUALITY (calidad del aire) ===
                if (airquality != nullptr && !doc["airquality"].isNull()) {
                    JsonObject aq = doc["airquality"];
                    airquality->aqi = aq["aqi"] | 0;
                    airquality->pm25 = aq["pm25"] | 0.0f;
                    airquality->pm10 = aq["pm10"] | 0.0f;  // May not be in response
                    strlcpy(airquality->station, aq["station"] | "WAQI", sizeof(airquality->station));
                    strlcpy(airquality->dominant, aq["dominant"] | "", sizeof(airquality->dominant));
                    airquality->valid = (airquality->aqi > 0);
                }

                // === STATS con horas de extremos ===
                if (stats != nullptr) {
                    JsonObject st = doc["stats"];
                    JsonObject temp_stats = st["temperature_outdoor"];
                    stats->temp_max = temp_stats["max"] | 0.0f;
                    stats->temp_min = temp_stats["min"] | 0.0f;

                    // Obtener timezone offset para convertir UTC a local
                    int tz = (tzOffset != nullptr) ? *tzOffset : -6;

                    // Helper lambda: extrae HH:MM de ISO y aplica timezone
                    auto extractLocalTime = [tz](const char* iso, char* out, size_t outSize) {
                        out[0] = '\0';
                        const char* t_pos = strchr(iso, 'T');
                        if (t_pos && strlen(t_pos) >= 6) {
                            int hour = (t_pos[1] - '0') * 10 + (t_pos[2] - '0');
                            int minute = (t_pos[4] - '0') * 10 + (t_pos[5] - '0');
                            // Aplicar timezone offset
                            hour += tz;
                            if (hour < 0) hour += 24;
                            if (hour >= 24) hour -= 24;
                            snprintf(out, outSize, "%02d:%02d", hour, minute);
                        }
                    };

                    const char* max_iso = temp_stats["max_time"] | "";
                    const char* min_iso = temp_stats["min_time"] | "";
                    extractLocalTime(max_iso, stats->temp_max_time, sizeof(stats->temp_max_time));
                    extractLocalTime(min_iso, stats->temp_min_time, sizeof(stats->temp_min_time));

                    JsonObject wind_stats = st["wind_gust"];
                    stats->wind_max = wind_stats["max"] | 0.0f;
                    const char* wind_iso = wind_stats["max_time"] | "";
                    extractLocalTime(wind_iso, stats->wind_max_time, sizeof(stats->wind_max_time));
                    stats->valid = true;
                }

                // === STATIONS (ch1 = jardin, gw1100 = remoto) ===
                JsonObject stations = doc["stations"];

                // Primero intentar stations.ch1, si no existe, buscar en current
                if (stations.containsKey("ch1")) {
                    JsonObject ch1 = stations["ch1"];
                    jardin.temperature = ch1["temperature"] | 0.0f;
                    jardin.humidity = ch1["humidity"] | 0.0f;
                    bool bat = ch1["battery"] | true;
                    jardin.battery = bat ? 100 : 10;
                    jardin.valid = true;
                    jardin.last_update = millis();
                } else if (!current["temperature_ch1"].isNull()) {
                    // Fallback: WN31 data en la sección current
                    jardin.temperature = current["temperature_ch1"] | 0.0f;
                    jardin.humidity = current["humidity_ch1"] | 0.0f;
                    bool bat = current["battery_ch1"] | true;
                    jardin.battery = bat ? 100 : 10;
                    jardin.valid = true;
                    jardin.last_update = millis();
                }

                if (stations.containsKey("gw1100")) {
                    JsonObject gw = stations["gw1100"];
                    remoto.temperature = gw["temperature"] | 0.0f;
                    remoto.humidity = gw["humidity"] | 0.0f;
                    remoto.pressure = gw["pressure"] | 0.0f;
                    remoto.valid = true;
                    remoto.last_update = millis();
                }

                http.end();
                Serial.println("[API] fetchAll OK (1 llamada)");
                return true;
            }
        }

        http.end();
        Serial.printf("[API] fetchAll error: %d\n", httpCode);
        return false;
    }

private:
    const char* _baseUrl;
};

// Instancia global
extern EcowittAPI ecowittApi;

#endif // ECOWITT_API_H
