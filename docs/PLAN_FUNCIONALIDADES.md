# Plan de Funcionalidades - ESP32 Display

## Inventario del Servidor Web (clima.xe1e.net)

### Endpoint Optimizado (Principal)

| Endpoint | Descripción | Estado |
|----------|-------------|--------|
| `/api/display` | **Endpoint optimizado para ESP32** - Combina todo en 1 llamada | ✅ En uso |

Contenido de `/api/display`:
- `timezone_offset`: Offset de zona horaria (ej: -6 para México)
- `current`: Lecturas actuales (temperatura, humedad, viento, lluvia, UV, etc.)
- `stats`: Min/Max del día por sensor
- `compare`: Diferencias vs ayer
- `almanac`: Amanecer, atardecer, fase lunar
- `forecast`: Pronóstico barométrico local
- `airquality`: AQI y PM2.5 (si configurado)
- `stations`: Datos de estaciones secundarias (ch1/WN31, gw1100)

### APIs Individuales (Referencia)

| Endpoint | Descripción | Estado ESP32 |
|----------|-------------|--------------|
| `/api/current` | Lecturas actuales de todos los sensores | ⚡ Via /api/display |
| `/api/stats/daily` | Min/Max/Avg del día | ⚡ Via /api/display |
| `/api/compare` | Comparación vs ayer | ⚡ Via /api/display |
| `/api/almanac` | Sol, luna, planetas | ⚡ Via /api/display |
| `/api/stations` | Lista de estaciones | ⚡ Via /api/display |
| `/api/forecast/local` | Pronóstico barométrico | ⚡ Via /api/display |
| `/api/airquality` | Calidad del aire (AQI) | ⚡ Via /api/display |
| `/api/alerts` | Alertas activas | ✅ Llamada separada |
| `/api/history` | Datos históricos | ❌ Pendiente |
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
- [x] Sol y luna (amanecer/atardecer/fase lunar)
- [x] Comparación vs ayer (delta temperatura)
- [x] Sincronización de hora desde servidor (con timezone)
- [x] 3 redes WiFi con fallback automático
- [x] Control de brillo (PWM)
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

## Arquitectura de Navegación

```
┌─────────────────────────────────────────────────────────────────────┐
│                           DASHBOARD                                  │
│                         (pantalla principal)                         │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌──────────┐  │
│  │Principal│  │ Jardín  │  │ Remoto  │  │Interior │  │  Config  │  │
│  │  (tap)  │  │  (tap)  │  │  (tap)  │  │  (tap)  │  │   (tap)  │  │
│  └────┬────┘  └────┬────┘  └────┬────┘  └─────────┘  └──────────┘  │
└───────┼────────────┼────────────┼───────────────────────────────────┘
        │            │            │
        ▼            ▼            ▼
┌───────────────┐ ┌─────────┐ ┌───────────────┐
│ ESTACIÓN      │ │ JARDÍN  │ │ REMOTO        │
│ PRINCIPAL     │ │ Detalle │ │ Detalle       │
│ (Fase 1+2)    │ │ simple  │ │               │
├───────────────┤ └─────────┘ ├───────────────┤
│ ← swipe →     │             │ ← swipe →     │
├───────────────┤             ├───────────────┤
│ HISTORIAL     │             │ HISTORIAL     │
│ Principal     │             │ Remoto        │
│ (Fase 3)      │             │ (Fase 3)      │
├───────────────┤             └───────────────┘
│ ← swipe →     │
├───────────────┤
│ RÉCORDS       │
│ (Fase 4)      │
├───────────────┤
│ ← swipe →     │
├───────────────┤
│ CALIDAD AIRE  │
│ (Fase 5)      │
├───────────────┤
│ ← swipe →     │
├───────────────┤
│ PRONÓSTICO    │
│ (Fase 5)      │
├───────────────┤
│ ← swipe →     │
├───────────────┤
│ AVIACIÓN      │
│ (Fase 5)      │
└───────────────┘
```

---

## Plan de Implementación por Fases

### Fase 1: Pantalla Detalle Estación Principal (Prioridad Alta)
Acceso: Tap en tarjeta Principal del Dashboard

**Contenido:**
1. Temperatura grande con sensación térmica
2. Pronóstico barométrico (texto predictivo)
3. Comparación vs ayer (deltas temp/humedad/presión)
4. Lluvia acumulada (día, semana, mes, año)
5. Radiación solar (W/m²)
6. Calidad del aire básica (AQI + color)
7. Min/Max con hora de los extremos

### Fase 2: Estadísticas Expandidas (Prioridad Alta)
Integrado en pantalla Detalle Principal

**Contenido:**
1. Min/Max/Avg de todos los sensores
2. Hora de los extremos
3. Escala Beaufort con descripción
4. Estado baterías por sensor

### Fase 3: Historial y Gráficas (Prioridad Media)
Dos pantallas separadas:

**3a. Historial Principal** (swipe desde Detalle Principal)
- Selector período (24h, 7d, 30d)
- Gráfica temperatura
- Gráfica presión  
- Gráfica lluvia
- Min/Max/Avg del período

**3b. Historial Remoto** (swipe desde Detalle Remoto)
- Selector período (24h, 7d, 30d)
- Gráfica temperatura
- Gráfica humedad
- Gráfica presión (si disponible)

### Fase 4: Climatología (Prioridad Media)
Acceso: Swipe desde Historial Principal

**Pantalla Récords:**
- Récords históricos (all-time)
- Récords del mes/año actual
- Rosa de vientos (visualización)
- Comparación mensual vs promedio

### Fase 5: Datos Externos (Prioridad Baja)
Acceso: Swipe desde Récords

**5a. Calidad del Aire Detallada:**
- PM2.5, PM10, O3, NO2
- Pronóstico AQI
- IMECA (si disponible)

**5b. Pronóstico Externo (Open-Meteo):**
- Próximas horas
- Próximos 5-7 días

**5c. Aviación (opcional):**
- METAR simplificado
- Condiciones de vuelo (VFR/IFR)

---

## Pantallas ESP32

### Dashboard (existente - sin cambios)
```
┌─────────────────────────────────────────────────────┐
│ Header: Estación, Hora, Status                      │
├─────────┬─────────┬─────────┬─────────┬─────────────┤
│ TEMP    │ HUMEDAD │ PRESIÓN │ VIENTO  │ SOL/LUNA    │
│ 23.4°C  │ 65%     │ 1013hPa │ 12km/h  │ ☀ 06:07    │
│ ↑25 ↓18 │         │ →estable│ NO 303° │ ☽ 19:18    │
├─────────┴─────────┴─────────┴─────────┴─────────────┤
│ SENSORES: Interior | Jardín | Remoto               │
└─────────────────────────────────────────────────────┘
```

### Detalle Estación Principal (Fase 1+2)
```
┌─────────────────────────────────────────────────────┐
│ ← ESTACIÓN PRINCIPAL                    [→ swipe]  │
├─────────────────────────────────────────────────────┤
│     23.4°C          │  Pronóstico: Estable         │
│   ST: 22.1°C        │  "Sin cambios próximas 12h"  │
│   ↓18.2 (06:15)     ├─────────────────────────────-─┤
│   ↑25.1 (14:30)     │  vs Ayer:  Δ-0.6°C           │
│                     │  Humedad:  Δ+5%              │
├─────────────────────┼──────────────────────────────┤
│  LLUVIA             │  RADIACIÓN                   │
│  Hoy: 0.0 mm        │  UV: 6 (Alto)                │
│  Sem: 12.4 mm       │  Solar: 845 W/m²             │
│  Mes: 45.2 mm       │                              │
│  Año: 234.5 mm      │  AQI: 45 (Bueno) ●           │
├─────────────────────┴──────────────────────────────┤
│  VIENTO: 12 km/h NO - Brisa ligera (Beaufort 3)   │
│  BATERÍAS: WS69 ██░ 60%  WN31 ███ 100%            │
└─────────────────────────────────────────────────────┘
```

### Detalle Jardín (simple)
```
┌─────────────────────────────────────────────────────┐
│ ← JARDÍN (WN31)                                    │
├─────────────────────────────────────────────────────┤
│                                                     │
│                     24.8°C                          │
│                      58%                            │
│                                                     │
│              ↓ 19.2°C    ↑ 28.4°C                  │
│                                                     │
│              Batería: ███░ 75%                      │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Detalle Remoto (GW1100)
```
┌─────────────────────────────────────────────────────┐
│ ← REMOTO (GW1100)                       [→ swipe]  │
├─────────────────────────────────────────────────────┤
│                                                     │
│         Temperatura: 22.3°C                         │
│         Humedad: 72%                                │
│         Presión: 1015.2 hPa                         │
│                                                     │
│         ↓ 18.5°C    ↑ 24.1°C                       │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### Historial Principal (Fase 3a)
```
┌─────────────────────────────────────────────────────┐
│ ← HISTORIAL PRINCIPAL       [24h] [7d] [30d]  [→] │
├─────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────┐   │
│  │         [Gráfica Temperatura]               │   │
│  └─────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────┐   │
│  │         [Gráfica Presión]                   │   │
│  └─────────────────────────────────────────────┘   │
│                                                     │
│  Período: Min 18.2°C  Max 25.1°C  Avg 21.5°C      │
└─────────────────────────────────────────────────────┘
```

### Historial Remoto (Fase 3b)
```
┌─────────────────────────────────────────────────────┐
│ ← HISTORIAL REMOTO          [24h] [7d] [30d]       │
├─────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────┐   │
│  │         [Gráfica Temperatura]               │   │
│  └─────────────────────────────────────────────┘   │
│  ┌─────────────────────────────────────────────┐   │
│  │         [Gráfica Humedad]                   │   │
│  └─────────────────────────────────────────────┘   │
│                                                     │
│  Período: Min 18.5°C  Max 24.1°C  Avg 20.8°C      │
└─────────────────────────────────────────────────────┘
```

### Récords (Fase 4)
```
┌─────────────────────────────────────────────────────┐
│ ← RÉCORDS                                     [→]  │
├─────────────────────────────────────────────────────┤
│ TEMPERATURA                                         │
│   Máxima histórica: 34.2°C (15-May-2024)           │
│   Mínima histórica:  5.1°C (12-Ene-2025)           │
├─────────────────────────────────────────────────────┤
│ VIENTO                                              │
│   Ráfaga máxima: 89.5 km/h (08-Sep-2024)           │
├─────────────────────────────────────────────────────┤
│ LLUVIA                                              │
│   Máx día: 45.2 mm (21-Jun-2024)                   │
│   Acum año: 523.4 mm                               │
├─────────────────────────────────────────────────────┤
│ [Rosa de Vientos]   Este mes vs promedio: +1.2°C   │
└─────────────────────────────────────────────────────┘
```

### Calidad del Aire (Fase 5a)
```
┌─────────────────────────────────────────────────────┐
│ ← CALIDAD DEL AIRE                            [→]  │
├─────────────────────────────────────────────────────┤
│                                                     │
│              AQI: 45                                │
│              BUENO ●                                │
│                                                     │
│  PM2.5: 12 μg/m³    PM10: 28 μg/m³                 │
│  O3: 45 ppb         NO2: 18 ppb                    │
│                                                     │
│  IMECA: 52 (Aceptable)                             │
│  Pronóstico: Sin cambios                           │
└─────────────────────────────────────────────────────┘
```

### Pronóstico (Fase 5b)
```
┌─────────────────────────────────────────────────────┐
│ ← PRONÓSTICO                                  [→]  │
├─────────────────────────────────────────────────────┤
│ PRÓXIMAS HORAS                                      │
│ 14:00  25°C ☀   15:00  26°C ☀   16:00  25°C ⛅    │
├─────────────────────────────────────────────────────┤
│ PRÓXIMOS DÍAS                                       │
│ Jue  ☀  18°/26°    Vie  ⛅  17°/24°               │
│ Sáb  🌧  16°/22°    Dom  ☀  15°/25°               │
│ Lun  ☀  16°/27°                                    │
└─────────────────────────────────────────────────────┘
```

### Aviación (Fase 5c)
```
┌─────────────────────────────────────────────────────┐
│ ← AVIACIÓN (MMMX)                                  │
├─────────────────────────────────────────────────────┤
│                     VFR ●                           │
│            Condiciones de vuelo visual              │
├─────────────────────────────────────────────────────┤
│ METAR MMMX 161753Z 36008KT 9999 FEW040 SCT100     │
│            25/08 A3012                              │
├─────────────────────────────────────────────────────┤
│ Viento: 360° @ 8kt    Visibilidad: >10km           │
│ Nubes: FEW 4000ft     Temp/Dewpoint: 25°C/8°C     │
│ Altímetro: 30.12 inHg                              │
└─────────────────────────────────────────────────────┘
```

---

## Estimación de Esfuerzo

| Fase | Pantallas | Complejidad | Tiempo Est. |
|------|-----------|-------------|-------------|
| 1 | Detalle Principal | Media | 2-3 días |
| 2 | Stats (integrado) | Baja | 1 día |
| 3a | Historial Principal | Alta | 3-4 días |
| 3b | Historial Remoto | Media | 2 días |
| 4 | Récords + Rosa | Media | 2-3 días |
| 5a | Calidad Aire | Baja | 1 día |
| 5b | Pronóstico | Media | 2 días |
| 5c | Aviación | Baja | 1 día |

**Total estimado: 2-3 semanas**

---

## Widgets Avanzados (LVGL)

| Widget | Uso | Componente LVGL |
|--------|-----|-----------------|
| Gráficas historial | Temperatura, presión, lluvia | `lv_chart` |
| Medidor presión | Barómetro circular con tendencia | `lv_meter` |
| Rosa de vientos | Distribución direccional animada | `lv_meter` + custom |
| Fase lunar | Indicador gráfico de luna | Custom canvas |
| Sparklines | Mini gráficas en cards | `lv_chart` (line) |

---

## Conectividad (Futuro)

| Feature | Descripción | Prioridad |
|---------|-------------|-----------|
| MQTT directo | Recibir datos sin polling HTTP | Media |
| BLE config | Configuración inicial via Bluetooth | Baja |
| OTA updates | Actualización firmware remota | Alta |

---

## Funcionalidad UX

| Feature | Descripción | Estado |
|---------|-------------|--------|
| Long-press | Acciones secundarias en cards | ❌ Pendiente |
| Slider brillo | Control PWM via CH422G | ✅ Implementado |
| Cache offline | Mostrar último dato si sin conexión | ❌ Pendiente |
| Reconexión WiFi | Automática con fallback 3 redes | ✅ Implementado |
| Swipe navegación | Cambio entre pantallas | ❌ Pendiente |
| Tap en cards | Acceso a pantallas detalle | ✅ Parcial |

---

## Notas Técnicas

### Hardware ESP32-S3
| Parámetro | Valor |
|-----------|-------|
| Pixel clock | 15 MHz (estable, no usar 30MHz) |
| Buffer LVGL | 200 líneas en PSRAM |
| I2C SDA | GPIO 8 |
| I2C SCL | GPIO 9 |
| Touch INT | GPIO 4 (LOW = touch activo) |
| Display | 1024x600 RGB |

### Fuentes Disponibles
```
Tamaños: 12, 14, 16, 18, 20, 24, 28, 36, 40, 48 px
Familia: Montserrat (LVGL default)
Weather Icons: Convertidos para LVGL
```

### Limitaciones
- RAM limitada para gráficas grandes → usar PSRAM
- Sin navegador → no imágenes satelitales/radar
- Pantalla 1024x600 → optimizar para ese tamaño

### APIs en ecowitt_api.h

**Implementadas:**
```cpp
// Endpoint optimizado - UNA llamada obtiene todo (en uso)
bool fetchAll(WeatherData&, CompareData&, AlmanacData&, 
              RemoteSensorData&, RemoteGatewayData&, int* tzOffset);

bool fetchAlerts(AlertData& data);              // /api/alerts (separado)
```

**Pendientes de implementar:**
```cpp
bool fetchHistory(HistoryData& data, int hours); // /api/history
bool fetchRecords(RecordsData& data);           // /api/climate/records
bool fetchWindRose(WindRoseData& data);         // /api/wind/rose
```

### Estructuras de datos a agregar en config.h
```cpp
struct AirQualityData { int aqi; float pm25, pm10, o3, no2; ... };
struct RecordsData { float temp_max, temp_min; char date_max[11]; ... };
struct HistoryPoint { time_t timestamp; float temp, humidity, pressure; };
struct HistoryData { HistoryPoint points[288]; int count; }; // 24h x 5min
```
