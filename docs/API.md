# API Reference - Ecowitt Weather Server

Documentación del API del servidor [ecowitt-weather-server-xe1e](https://github.com/XE1E/ecowitt-weather-server-xe1e).

## Base URL

```
https://clima.xe1e.net
```

## Endpoint Optimizado (ESP32)

### `GET /api/display`

**Endpoint principal para el ESP32.** Combina múltiples APIs en una sola llamada.

```json
{
  "timezone_offset": -6,
  "current": {
    "temperature_outdoor": 25.3,
    "humidity_outdoor": 65,
    "pressure_relative": 1013.2,
    "wind_speed": 12.5,
    "wind_gust": 18.2,
    "wind_direction": 180,
    "rain_rate": 0.0,
    "rain_daily": 2.5,
    "uv_index": 5,
    "solar_radiation": 450.2,
    "feels_like": 26.1,
    "dew_point": 18.2,
    "temperature_indoor": 22.1,
    "humidity_indoor": 45,
    "battery_wh65": true,
    "received_at": "2026-07-16T14:30:00"
  },
  "stats": {
    "temperature_outdoor": { "min": 18.2, "max": 28.5, "avg": 23.4 },
    "wind_gust": { "max": 35.2 },
    "rain_daily": { "max": 5.2 }
  },
  "compare": {
    "temp_diff": 2.3,
    "humidity_diff": -5,
    "pressure_diff": 1.2
  },
  "almanac": {
    "sunrise": "06:07",
    "sunset": "20:18",
    "moon_phase": "Cuarto creciente",
    "moon_illumination": 45
  },
  "forecast": {
    "trend": "stable",
    "description": "Sin cambios significativos"
  },
  "airquality": {
    "aqi": 45,
    "pm25": 12.5,
    "dominant": "pm25"
  },
  "stations": {
    "ch1": {
      "temperature": 24.5,
      "humidity": 58,
      "battery": true
    },
    "gw1100": {
      "temperature": 22.3,
      "humidity": 72,
      "pressure": 1015.2
    }
  },
  "generated_at": "2026-07-16T14:30:01"
}
```

| Campo | Descripción |
|-------|-------------|
| `timezone_offset` | Offset UTC en horas (ej: -6 para México) |
| `current` | Lecturas actuales de la estación principal |
| `stats` | Min/Max/Avg del día por sensor |
| `compare` | Diferencias vs mismo momento ayer |
| `almanac` | Amanecer, atardecer, fase lunar |
| `forecast` | Pronóstico barométrico local |
| `airquality` | Calidad del aire (si configurado) |
| `stations` | Estaciones secundarias (ch1=WN31, gw1100) |

---

## Endpoints Individuales

| Método | Endpoint | Descripción | Estado ESP32 |
|--------|----------|-------------|--------------|
| GET | `/api/display` | **Endpoint optimizado** | ✅ Principal |
| GET | `/api/current` | Lecturas actuales | ⚡ Via display |
| GET | `/api/stats/daily` | Estadísticas del día | ⚡ Via display |
| GET | `/api/compare` | Comparación vs ayer | ⚡ Via display |
| GET | `/api/almanac` | Sol, luna, planetas | ⚡ Via display |
| GET | `/api/forecast/local` | Pronóstico barométrico | ⚡ Via display |
| GET | `/api/airquality` | Calidad del aire (AQI) | ⚡ Via display |
| GET | `/api/stations` | Lista de estaciones | ⚡ Via display |
| GET | `/api/alerts` | Alertas meteorológicas | ✅ Separado |
| GET | `/api/history` | Datos históricos | ❌ Pendiente |
| GET | `/api/climate/records` | Récords históricos | ❌ Pendiente |
| GET | `/api/climate/daily` | Resúmenes diarios | ❌ Pendiente |
| GET | `/api/wind/rose` | Rosa de vientos | ❌ Pendiente |
| GET | `/api/metar?station=MMMX` | METAR aviación | ❌ Pendiente |
| GET | `/api/taf?station=MMMX` | TAF aviación | ❌ Pendiente |
| GET | `/api/airquality/imeca` | Índice IMECA | ❌ Pendiente |
| POST | `/data/report/` | Recibe push de estaciones | N/A |
| GET | `/health` | Estado del servicio | N/A |

---

## Multi-Estación

Los endpoints aceptan `?station=<id>` para consultar estaciones secundarias:

```
GET /api/current                  → Estación principal
GET /api/current?station=gw1100   → GW1100 (remoto)
GET /api/current?station=ch1      → Canal 1 WN31 (jardín)
```

---

## Respuestas Detalladas

### `/api/current`

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
  "rain_rate": 0.0,
  "rain_daily": 2.5,
  "rain_weekly": 15.3,
  "rain_monthly": 45.2,
  "rain_yearly": 320.5,
  "solar_radiation": 450.2,
  "uv_index": 5,
  "dew_point": 18.2,
  "feels_like": 26.1,
  "battery_wh65": true,
  "received_at": "2026-07-16T14:30:00"
}
```

### `/api/stats/daily`

```json
{
  "stats": {
    "temperature_outdoor": { "min": 18.2, "max": 28.5, "avg": 23.4 },
    "humidity_outdoor": { "min": 45, "max": 85, "avg": 62 },
    "wind_gust": { "max": 35.2 },
    "rain_daily": { "max": 5.2 },
    "uv_index": { "max": 8 }
  }
}
```

### `/api/compare`

```json
{
  "temp_diff": 2.3,
  "humidity_diff": -5,
  "pressure_diff": 1.2
}
```

### `/api/almanac`

```json
{
  "sun": {
    "rise": "06:07",
    "set": "20:18",
    "solar_noon": "13:12",
    "day_length": "14:11"
  },
  "moon": {
    "phase": "Cuarto creciente",
    "illumination": 45,
    "rise": "12:30",
    "set": "00:15"
  }
}
```

### `/api/alerts`

```json
{
  "alerts": [
    {
      "type": "heat",
      "severity": "moderate",
      "message": "Temperatura alta esperada",
      "expires": "2026-07-16T18:00:00Z"
    }
  ]
}
```

### `/api/history`

Parámetros: `?start=-24h` o `?start=2026-07-15T00:00:00&end=2026-07-16T00:00:00`

```json
{
  "points": [
    { "time": "2026-07-16T00:00:00", "temperature_outdoor": 18.5, "humidity_outdoor": 78 },
    { "time": "2026-07-16T01:00:00", "temperature_outdoor": 18.2, "humidity_outdoor": 80 }
  ]
}
```

---

## Hardware de Estaciones

### Estación Principal (WS2910 + WS69)
| Componente | Datos |
|------------|-------|
| WS2910 (consola) | temp_indoor, humidity_indoor |
| WS69 (exterior) | temp_outdoor, humidity, viento, lluvia, UV, solar |
| WN31 (canales 1-8) | temperature_chX, humidity_chX, battery_chX |

### Estación Secundaria (GW1100)
| Componente | Datos |
|------------|-------|
| GW1100 (gateway) | temperature_indoor, humidity_indoor, pressure |

---

## Unidades

| Medida | Unidad |
|--------|--------|
| Temperatura | °C |
| Humedad | % |
| Presión | hPa (mbar) |
| Viento | km/h |
| Lluvia | mm |
| Radiación solar | W/m² |
| UV | Índice (0-11+) |

---

## Notas

- `received_at` está en UTC, usar `timezone_offset` para convertir a local
- `battery_wh65`: true = OK, false = baja
- `battery_chX`: 1 = OK, 0 = baja (sensores WN31)
- AQI solo disponible si `waqi_token` está configurado en el servidor
