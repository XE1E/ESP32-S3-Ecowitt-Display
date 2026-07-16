# API Reference - Ecowitt Weather Server

Documentación del API del servidor [ecowitt-weather-server-xe1e](https://github.com/XE1E/ecowitt-weather-server-xe1e).

## Base URL

```
https://clima.xe1e.net
```

## Endpoints Principales

| Método | Endpoint | Descripción |
|--------|----------|-------------|
| POST | `/data/report/` | Recibe push de estaciones Ecowitt |
| GET | `/api/current` | Lecturas actuales |
| GET | `/api/history` | Datos históricos |
| GET | `/api/stats/daily` | Estadísticas del día (max/min/avg) |
| GET | `/api/compare` | Comparación vs ayer |
| GET | `/api/climate/daily` | Resúmenes diarios |
| GET | `/api/climate/records` | Records históricos |
| GET | `/api/forecast/local` | Pronóstico barométrico |
| GET | `/api/almanac` | Sol, luna, planetas |
| GET | `/api/alerts` | Alertas meteorológicas |
| GET | `/api/airquality` | Calidad del aire (AQI) |
| GET | `/api/airquality/imeca` | Índice IMECA |
| GET | `/api/metar?station=MMMX` | METAR aviación |
| GET | `/api/taf?station=MMMX` | TAF aviación |
| GET | `/api/satellite` | Imágenes satelitales |
| GET | `/api/stations` | Lista de estaciones |
| GET | `/health` | Estado del servicio |

## Multi-Estación

Los endpoints `/api/current`, `/api/history` y `/api/stats/daily` aceptan el parámetro `?station=<nombre>` para consultar estaciones secundarias:

```
GET /api/current                  → Estación principal
GET /api/current?station=gw1100   → Estación secundaria GW1100
```

## Respuesta de `/api/current`

```json
{
  "temperature_outdoor": 25.3,
  "temperature_indoor": 22.1,
  "humidity_outdoor": 65,
  "humidity_indoor": 45,
  "pressure_relative": 1013.2,
  "pressure_absolute": 1010.5,
  "wind_speed": 12.5,
  "wind_gust": 18.2,
  "wind_direction": 180,
  "wind_dir_cardinal": "S",
  "rain_rate": 0.0,
  "rain_daily": 2.5,
  "rain_weekly": 15.3,
  "rain_monthly": 45.2,
  "rain_yearly": 320.5,
  "solar_radiation": 450.2,
  "uv_index": 5,
  "dew_point": 18.2,
  "feels_like": 26.1,
  "humidex": 27.3,
  "cloud_base": 890,
  "station_type": "WS2910_V1.0.0",
  "model": "WS69",
  "received_at": "2024-01-15T14:30:00.000Z"
}
```

### Canales WN31 (1-8)

Si hay sensores WN31 conectados, se incluyen campos adicionales:

```json
{
  "temperature_ch1": 24.5,
  "humidity_ch1": 58,
  "battery_ch1": 1,
  "temperature_ch2": 26.1,
  "humidity_ch2": 52,
  "battery_ch2": 1
}
```

- `temperature_chX`: Temperatura del canal X (°C)
- `humidity_chX`: Humedad del canal X (%)
- `battery_chX`: Estado batería (1=OK, 0=baja)

## Respuesta de `/api/stats/daily`

```json
{
  "temp_max": 28.5,
  "temp_min": 18.2,
  "temp_avg": 23.4,
  "humidity_max": 85,
  "humidity_min": 45,
  "wind_max": 35.2,
  "rain_total": 5.2,
  "uv_max": 8
}
```

## Respuesta de `/api/compare`

```json
{
  "temp_diff": 2.3,
  "humidity_diff": -5,
  "pressure_diff": 1.2
}
```

## Respuesta de `/api/almanac`

```json
{
  "sun": {
    "sunrise": "06:42",
    "sunset": "20:15",
    "solar_noon": "13:28",
    "day_length": "13:33"
  },
  "moon": {
    "phase": "Cuarto creciente",
    "illumination": 45,
    "moonrise": "12:30",
    "moonset": "00:15"
  }
}
```

## Respuesta de `/api/alerts`

```json
{
  "alerts": [
    {
      "type": "heat",
      "severity": "moderate",
      "message": "Temperatura alta esperada",
      "expires": "2024-01-15T18:00:00Z"
    }
  ]
}
```

## Respuesta de `/api/stations`

```json
{
  "stations": [
    {"id": "principal", "name": "Estación Principal", "model": "WS2910+WS69"},
    {"id": "gw1100", "name": "Estación Remota", "model": "GW1100"}
  ]
}
```

## Configuración de Hardware

### Estación Principal
- **WS2910**: Consola interior (temp_indoor, humidity_indoor)
- **WS69**: Sensor exterior (temp_outdoor, humidity_outdoor, viento, lluvia, UV, solar)
- **WN31**: Sensores adicionales T/H (hasta 8 canales)

### Estación Secundaria
- **GW1100**: Gateway WiFi con sensores propios
- Accesible via `?station=gw1100`

## Notas

- Todas las temperaturas en °C
- Presión en hPa (mbar)
- Viento en km/h
- Lluvia en mm
- Radiación solar en W/m²
- Los campos de batería son booleanos (1=OK, 0=baja) o porcentaje según el sensor
