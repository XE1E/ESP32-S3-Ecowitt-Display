# Plan de Funcionalidades - ESP32 Display

## Inventario del Servidor Web (clima.xe1e.net)

### APIs Disponibles

| Endpoint | Descripción | Estado ESP32 |
|----------|-------------|--------------|
| `/api/current` | Lecturas actuales de todos los sensores | ✅ Implementado |
| `/api/stats/daily` | Min/Max/Avg del día | ✅ Implementado |
| `/api/compare` | Comparación vs ayer | ✅ Implementado |
| `/api/alerts` | Alertas activas | ✅ Implementado |
| `/api/almanac` | Sol, luna, planetas | ✅ Implementado |
| `/api/stations` | Lista de estaciones | ✅ Implementado |
| `/api/forecast/local` | Pronóstico barométrico | ⚠️ Parcial |
| `/api/history` | Datos históricos | ❌ Pendiente |
| `/api/airquality` | Calidad del aire (AQI) | ❌ Pendiente |
| `/api/airquality/imeca` | Índice IMECA | ❌ Pendiente |
| `/api/climate/records` | Récords históricos | ❌ Pendiente |
| `/api/climate/daily` | Resúmenes diarios | ❌ Pendiente |
| `/api/wind/rose` | Rosa de vientos | ❌ Pendiente |
| `/api/metar` | METAR aviación | ❌ Pendiente |
| `/api/taf` | TAF aviación | ❌ Pendiente |
| `/api/satellite` | Imágenes satelitales | ❌ No viable |

---

## Lecturas de Sensores Disponibles

### Temperatura
- [x] Exterior (temperature_outdoor)
- [x] Interior (temperature_indoor)
- [x] Canal 1-8 WN31 (temperature_ch1..8)
- [x] Sensación térmica (feels_like)
- [x] Punto de rocío (dew_point)
- [ ] Wind chill
- [ ] Humidex

### Humedad
- [x] Exterior (humidity_outdoor)
- [x] Interior (humidity_indoor)
- [x] Canales WN31 (humidity_ch1..8)

### Presión
- [x] Relativa (pressure_relative)
- [x] Tendencia (trend)
- [ ] Absoluta (pressure_absolute)

### Viento
- [x] Velocidad (wind_speed)
- [x] Ráfaga (wind_gust)
- [x] Dirección grados (wind_dir)
- [x] Dirección cardinal
- [ ] Escala Beaufort con descripción

### Precipitación
- [x] Tasa actual (rain_rate)
- [x] Acumulado día (rain_daily)
- [ ] Acumulado semana (rain_weekly)
- [ ] Acumulado mes (rain_monthly)
- [ ] Acumulado año (rain_yearly)

### Radiación
- [x] UV Index
- [ ] Radiación solar (solar_radiation)
- [ ] Altura base nubes (cloud_base)

### Batería Sensores
- [ ] Estado batería por sensor

---

## Estadísticas Disponibles (api/stats/daily)

| Sensor | Min | Max | Avg | Estado |
|--------|-----|-----|-----|--------|
| Temp exterior | ✅ | ✅ | ❌ | Parcial |
| Humedad exterior | ❌ | ❌ | ❌ | Pendiente |
| Presión | ❌ | ❌ | ❌ | Pendiente |
| Viento | ❌ | ✅ | ❌ | Parcial |
| Ráfaga | ❌ | ✅ | ❌ | Parcial |
| UV | ❌ | ❌ | ❌ | Pendiente |
| Radiación solar | ❌ | ❌ | ❌ | Pendiente |
| Lluvia día | ❌ | ❌ | ❌ | Pendiente |

---

## Funcionalidades del Servidor Web

### Dashboard Principal
- [x] Condiciones actuales
- [x] Temperatura con max/min
- [x] Humedad
- [x] Presión con tendencia
- [x] Viento con dirección
- [x] Sol y luna
- [ ] Comparación vs ayer (delta)
- [ ] Pronóstico barométrico texto
- [ ] Widget calidad aire

### Pantallas Adicionales Web
- [ ] **Historial**: Gráficas 24h/7d/30d
- [ ] **Estadísticas**: Resumen anual
- [ ] **Climatología**: Récords, promedios mensuales
- [ ] **Pronóstico**: Hourly/Daily de Open-Meteo
- [ ] **Rosa de Vientos**: Distribución direccional
- [ ] **Calidad Aire**: AQI + contaminantes
- [ ] **Aviación**: METAR/TAF decodificado
- [ ] **Satélite/Radar**: Imágenes (no viable en ESP32)

---

## Plan de Implementación por Fases

### Fase 1: Completar Dashboard Principal (Prioridad Alta)
1. **Pronóstico barométrico** - Mostrar texto predictivo
2. **Comparación vs ayer** - Deltas de temperatura/humedad
3. **Calidad del aire básica** - AQI + color indicador
4. **Lluvia acumulada** - Semana, mes, año
5. **Radiación solar** - W/m² actual

### Fase 2: Estadísticas del Día (Prioridad Alta)
1. **Panel estadísticas expandido**:
   - Min/Max/Avg de todos los sensores
   - Hora de los extremos
2. **Escala Beaufort** - Descripción del viento
3. **Estado baterías** - Indicadores por sensor

### Fase 3: Historial y Gráficas (Prioridad Media)
1. **Mini gráficas en cards** - Sparklines 24h
2. **Pantalla historial dedicada**:
   - Selector período (24h, 7d, 30d)
   - Gráfica temperatura
   - Gráfica presión
   - Gráfica lluvia
3. **Almacenar datos localmente** - Buffer circular en PSRAM

### Fase 4: Climatología (Prioridad Media)
1. **Pantalla récords**:
   - Récords históricos (all-time)
   - Récords del mes/año
2. **Rosa de vientos** - Visualización gráfica
3. **Comparación mensual** - Este mes vs promedio

### Fase 5: Datos Externos (Prioridad Baja)
1. **Calidad aire detallada**:
   - PM2.5, PM10, O3, NO2
   - Pronóstico AQI
   - IMECA (si disponible)
2. **Pronóstico externo** (Open-Meteo):
   - Próximas horas
   - Próximos días
3. **Aviación** (opcional):
   - METAR simplificado
   - Condiciones de vuelo

---

## Pantallas Propuestas para ESP32

### Pantalla Principal (existente)
```
┌─────────────────────────────────────────────────────┐
│ Header: Estación, Hora, Status                      │
├─────────┬─────────┬─────────┬─────────┬─────────────┤
│ TEMP    │ HUMEDAD │ PRESIÓN │ VIENTO  │ SOL/LUNA    │
│ 23.4°C  │ 65%     │ 1013hPa │ 12km/h  │ ☀️ 06:07    │
│ ↑25 ↓18 │         │ →estable│ NO 303° │ 🌙 19:18    │
├─────────┴─────────┴─────────┴─────────┴─────────────┤
│ SENSORES: Portable | Jardín | Remoto | [+AQI]      │
└─────────────────────────────────────────────────────┘
```

### Pantalla Detalle (tap en card)
```
┌─────────────────────────────────────────────────────┐
│ ← TEMPERATURA                                       │
├─────────────────────────────────────────────────────┤
│                    23.4°C                           │
│              Sensación: 22.1°C                      │
│                                                     │
│  ┌─────────────────────────────────────────────┐   │
│  │ [Sparkline 24h]                              │   │
│  └─────────────────────────────────────────────┘   │
│                                                     │
│  Hoy: ↓18.2°C (06:15)  ↑25.1°C (14:30)            │
│  Ayer: 24.0°C  (Δ -0.6°C)                         │
│  Promedio mes: 22.3°C                              │
└─────────────────────────────────────────────────────┘
```

### Pantalla Historial (nueva)
```
┌─────────────────────────────────────────────────────┐
│ ← HISTORIAL              [24h] [7d] [30d]          │
├─────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────┐   │
│  │                                              │   │
│  │         [Gráfica Temperatura]               │   │
│  │                                              │   │
│  └─────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────┐   │
│  │         [Gráfica Presión]                   │   │
│  └─────────────────────────────────────────────┘   │
│                                                     │
│  Min: 18.2°C  Max: 25.1°C  Avg: 21.5°C            │
└─────────────────────────────────────────────────────┘
```

### Pantalla Récords (nueva)
```
┌─────────────────────────────────────────────────────┐
│ ← RÉCORDS                                           │
├─────────────────────────────────────────────────────┤
│ TEMPERATURA                                         │
│   Máxima histórica: 34.2°C (15-May-2024)           │
│   Mínima histórica:  5.1°C (12-Ene-2025)           │
│   Mayor rango día:  18.3°C (23-Mar-2024)           │
├─────────────────────────────────────────────────────┤
│ VIENTO                                              │
│   Ráfaga máxima: 89.5 km/h (08-Sep-2024)           │
├─────────────────────────────────────────────────────┤
│ LLUVIA                                              │
│   Máx en un día: 45.2 mm (21-Jun-2024)             │
│   Acumulado año: 523.4 mm                          │
└─────────────────────────────────────────────────────┘
```

---

## Estimación de Esfuerzo

| Fase | Tareas | Complejidad | Tiempo Est. |
|------|--------|-------------|-------------|
| 1 | 5 | Baja | 2-3 días |
| 2 | 3 | Baja | 1-2 días |
| 3 | 3 | Alta | 4-5 días |
| 4 | 3 | Media | 2-3 días |
| 5 | 3 | Media | 2-3 días |

**Total estimado: 2-3 semanas**

---

## Notas Técnicas

### Limitaciones ESP32
- RAM limitada para gráficas grandes → usar PSRAM
- Sin navegador → no imágenes satelitales/radar
- Pantalla 1024x600 → optimizar para ese tamaño

### APIs a agregar en ecowitt_api.h
```cpp
bool fetchAirQuality(AirQualityData& data);     // /api/airquality
bool fetchHistory(HistoryData& data, int hours); // /api/history
bool fetchRecords(RecordsData& data);           // /api/climate/records
bool fetchWindRose(WindRoseData& data);         // /api/wind/rose
bool fetchForecast(ForecastData& data);         // /api/forecast
```

### Estructuras de datos a agregar en config.h
```cpp
struct AirQualityData { int aqi; float pm25, pm10, o3, no2; ... };
struct RecordsData { float temp_max, temp_min; char date_max[11]; ... };
struct HistoryPoint { time_t timestamp; float temp, humidity, pressure; };
struct HistoryData { HistoryPoint points[288]; int count; }; // 24h x 5min
```
