# Plan de Desarrollo — ESP32-S3 Ecowitt Display

> Estado: **planificación detallada**
> Objetivo: Display táctil de 7" que muestre datos de la estación meteorológica Ecowitt en tiempo real.

---

## 1. Hardware

### 1.1 Especificaciones del Display

| Componente | Especificación |
|------------|----------------|
| **Modelo** | Waveshare ESP32-S3-Touch-LCD-7B (Type B) |
| **Display** | 7" IPS LCD, **1024×600** píxeles |
| **Interfaz** | RGB paralelo (no SPI) |
| **Touch** | GT911 capacitivo multi-touch (5 puntos) |
| **CPU** | ESP32-S3 dual-core Xtensa LX7 @ 240MHz |
| **Flash** | 16MB |
| **PSRAM** | 8MB (OPI/Octal mode) — **crítico para buffer de pantalla** |
| **WiFi** | 802.11 b/g/n 2.4GHz |
| **USB** | Type-C (CDC + JTAG) |
| **Alimentación** | 5V via USB-C |

### 1.2 Pinout crítico

| Función | GPIO | Notas |
|---------|------|-------|
| Touch SDA | 19 | I2C compartido |
| Touch SCL | 20 | I2C compartido |
| Touch RST | 38 | Reset del GT911 |
| Touch INT | 18 | Interrupción touch |
| LCD Backlight | 2 | PWM para brillo |
| BME280 SDA | 19 | Compartido con touch |
| BME280 SCL | 20 | Compartido con touch |

### 1.3 RGB LCD Timing (Type B - 1024×600)

```cpp
// Parámetros de timing para Arduino_ESP32RGBPanel
hsync_polarity: 1
hsync_front_porch: 46
hsync_pulse_width: 2
hsync_back_porch: 44
vsync_polarity: 1
vsync_front_porch: 10
vsync_pulse_width: 16
vsync_back_porch: 16
PCLK: ~30 MHz
```

### 1.4 Sensor interior (opcional)

| Sensor | Mediciones |
|--------|------------|
| **BME280** | Temperatura, humedad, presión barométrica |
| **Conexión** | I2C (comparte bus con touch) |
| **Dirección** | 0x76 o 0x77 |

---

## 2. Software Stack

### 2.1 Configuración Arduino IDE

| Setting | Valor | Notas |
|---------|-------|-------|
| Board | ESP32S3 Dev Module | |
| USB CDC On Boot | **Enable** | Para Serial debug |
| USB DFU On Boot | Disable | |
| Flash Size | **16MB (128Mb)** | |
| Flash Mode | **QIO 80MHz** | |
| PSRAM | **OPI PSRAM** | ⚠️ Crítico |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) | |
| Upload Speed | 921600 | |

### 2.2 Librerías requeridas

| Librería | Versión | Función |
|----------|---------|---------|
| esp32 (Espressif) | **2.0.17** | Board support (NO usar 3.x) |
| lvgl | **8.3.x** | UI framework (NO usar 9.x) |
| GFX Library for Arduino | **1.3.7** | Display driver (versión exacta) |
| ArduinoJson | 6.x o 7.x | Parser JSON |
| Adafruit BME280 | Latest | Sensor interior |

### 2.3 Configuración crítica — Anti-Tearing

```cpp
// lvgl_port.h — SOLO usar Mode 1
#define LVGL_PORT_AVOID_TEAR_ENABLE     (1)
#define LVGL_PORT_AVOID_TEAR_MODE       (1)  // Double-buffer + full-refresh

// ⚠️ Mode 2 = CRASH
// ⚠️ Mode 3 = Artefactos visuales
```

### 2.4 Configuración LVGL (lv_conf.h)

```c
#define LV_HOR_RES_MAX          1024
#define LV_VER_RES_MAX          600
#define LV_COLOR_DEPTH          16
#define LV_COLOR_16_SWAP        0
#define LV_MEM_CUSTOM           1      // Usar PSRAM
#define LV_USE_PERF_MONITOR     0      // Desactivar en producción

// Fuentes habilitadas
#define LV_FONT_MONTSERRAT_12   1
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_18   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_24   1
#define LV_FONT_MONTSERRAT_28   1
#define LV_FONT_MONTSERRAT_48   1
```

---

## 3. Arquitectura de Software

### 3.1 Flujo principal

```
┌─────────────────────────────────────────────────────────────────────┐
│                           BOOT                                      │
│                             │                                       │
│                             ▼                                       │
│    ┌─────────────────────────────────────────────────────────┐      │
│    │  1. Inicializar PSRAM (verificar 8MB disponibles)       │      │
│    │  2. Inicializar I2C (GPIO 19/20)                        │      │
│    │  3. Inicializar Display RGB (1024×600)                  │      │
│    │  4. Inicializar Touch GT911                             │      │
│    │  5. Inicializar LVGL con doble buffer                   │      │
│    └─────────────────────────────────────────────────────────┘      │
│                             │                                       │
│                             ▼                                       │
│    ┌─────────────────────────────────────────────────────────┐      │
│    │  Mostrar Splash Screen                                  │      │
│    │  "Conectando WiFi..."                                   │      │
│    └─────────────────────────────────────────────────────────┘      │
│                             │                                       │
│                             ▼                                       │
│    ┌─────────────────────────────────────────────────────────┐      │
│    │  ¿WiFi configurado en NVS?                              │      │
│    │     SÍ → Conectar                                       │      │
│    │     NO → Mostrar Wizard de configuración                │      │
│    └─────────────────────────────────────────────────────────┘      │
│                             │                                       │
│                             ▼                                       │
│    ┌─────────────────────────────────────────────────────────┐      │
│    │  Sincronizar NTP (hora local)                           │      │
│    │  Fetch inicial de datos (/api/current)                  │      │
│    │  Crear UI principal                                     │      │
│    └─────────────────────────────────────────────────────────┘      │
│                             │                                       │
│                             ▼                                       │
│  ┌───────────────────────────────────────────────────────────────┐  │
│  │                     LOOP PRINCIPAL                            │  │
│  │                                                               │  │
│  │   ┌─────────────────────────────────────────────────────┐     │  │
│  │   │  lv_timer_handler()  ← Procesa UI y touch           │     │  │
│  │   └─────────────────────────────────────────────────────┘     │  │
│  │                          │                                    │  │
│  │   ┌──────────────────────┴──────────────────────┐             │  │
│  │   │                                             │             │  │
│  │   ▼                                             ▼             │  │
│  │  Cada 1s:                                   Cada 60s:         │  │
│  │  - Actualizar reloj                         - Fetch API       │  │
│  │  - Leer BME280 (si existe)                  - Actualizar UI   │  │
│  │                                             - Check alertas   │  │
│  │                                                               │  │
│  │   Cada 10 min:                              Cada 30 min:      │  │
│  │   - Fetch /api/almanac                      - Fetch /forecast │  │
│  │                                                               │  │
│  └───────────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────────┘
```

### 3.2 Estructura de archivos

```
ESP32-S3-Ecowitt-Display/
├── ESP32-S3-Ecowitt-Display.ino   # Entry point
│
├── config.h                        # Estructuras de datos
├── my_config.h                     # Configuración usuario (gitignore)
├── my_config.h.template            # Plantilla
│
├── ecowitt_api.h                   # Cliente REST para API propia ✓
│
├── lvgl_port.h                     # Configuración LVGL ✓
├── lvgl_port.cpp                   # Inicialización LVGL ✓
├── rgb_lcd_port.h                  # Driver display RGB ✓
├── rgb_lcd_port.cpp                # ✓
├── gt911.h                         # Driver touch ✓
├── gt911.cpp                       # ✓
├── touch.h                         # Abstracción touch ✓
├── touch.cpp                       # ✓
├── i2c.h                           # Bus I2C ✓
├── i2c.cpp                         # ✓
├── io_extension.h                  # PWM backlight ✓
├── io_extension.cpp                # ✓
│
├── ui.h                            # Controlador UI principal ✓
├── ui_dashboard.h                  # Pantalla principal (NUEVO)
├── ui_settings.h                   # Pantalla ajustes (NUEVO)
├── ui_alerts.h                     # Panel de alertas (NUEVO)
│
├── keyboard_ui.h                   # Teclado en pantalla ✓
├── setup_wizard.h                  # Wizard de configuración ✓
│
├── bme280_sensor.h                 # Sensor interior ✓
├── ntp_time.h                      # Sincronización horaria ✓
├── localization.h                  # Textos en español ✓
│
├── weather_icons.h                 # Iconos del clima ✓
├── weather_icon_data.h             # Bitmaps ✓
│
├── lv_conf.h                       # Config LVGL (copiar a libraries/) ✓
│
├── PLAN.md                         # Este documento
├── README.md                       # ✓
├── CLAUDE.md                       # ✓
└── .gitignore                      # ✓
```

---

## 4. API Integration

### 4.1 Endpoints del servidor Ecowitt

| Endpoint | Datos | Frecuencia |
|----------|-------|------------|
| `GET /api/current` | Temp, humedad, viento, lluvia, UV, presión, interior | 60s |
| `GET /api/stats/daily` | Máx/mín del día, totales | 60s |
| `GET /api/compare` | Diferencia vs ayer | 60s |
| `GET /api/alerts` | Alertas activas | 60s |
| `GET /api/forecast` | Pronóstico 3-7 días | 30 min |
| `GET /api/almanac` | Sol, luna, planetas | 10 min |
| `GET /health` | Estado del servidor | Al conectar |

### 4.2 Estructura de respuesta /api/current

```json
{
  "temperature_outdoor": 21.5,
  "humidity_outdoor": 65,
  "pressure_rel": 1018.2,
  "wind_speed": 12.5,
  "wind_gust": 18.2,
  "wind_dir": 45,
  "wind_dir_cardinal": "NE",
  "rain_rate": 0.0,
  "rain_day": 2.5,
  "uv": 3,
  "solar_radiation": 450,
  "temperature_indoor": 22.3,
  "humidity_indoor": 58,
  "feels_like": 20.8,
  "dewpoint": 14.2,
  "timestamp": "2026-07-13T16:45:00Z"
}
```

### 4.3 Diferencias vs WeatherAPI.com (proyecto anterior)

| Aspecto | WeatherAPI.com | clima.xe1e.net |
|---------|----------------|----------------|
| Límite de llamadas | 1M/mes (gratis) | Sin límites |
| Datos | Interpolados/modelados | **Reales de tu estación** |
| Interior | No disponible | ✓ de la consola |
| Alertas | Genéricas | **Tus alertas configuradas** |
| Costo | Potencial ($) | Gratis |
| Latencia | ~200-500ms | ~50-100ms (local) |
| Dependencia | Externa | **Tu servidor** |

---

## 5. Diseño de UI

### 5.1 Layout principal (1024×600)

```
┌──────────────────────────────────────────────────────────────────────────────┐
│ HEADER (50px)                                                                │
│  🟢 Online   │   📡 -65 dBm   │   ⚙️                              16:48:32   │
├──────────────┬───────────────┬───────────────────────────────────────────────┤
│              │               │                                               │
│    RELOJ     │   PRESIÓN     │              ALERTAS / TENDENCIAS             │
│              │               │                                               │
│   16:48      │  1018.2 hPa   │  🟢 Sin alertas activas                       │
│              │    ↗ +1.5     │                                               │
│  Dom 13 Jul  │   (3h)        │  Temp: +0.8°C vs ayer                         │
│              │               │  Pres: -2.1 hPa/3h (bajando)                  │
│   (200px)    │   (160px)     │                              (restante)       │
│              │               │                                               │
├──────────────┼───────────────┼───────────────────────────────────────────────┤
│              │               │                                               │
│ TEMPERATURA  │   HUMEDAD     │            CONDICIONES EXTERIORES             │
│              │               │                                               │
│   21.5°C     │    ┌───┐      │  ☀️  DESPEJADO                                │
│              │    │   │      │                                               │
│  sensación   │    │65%│      │  💨 12 km/h NE  ·  Ráfaga: 18 km/h           │
│   20.2°C     │    │   │      │  ☀️ UV: 3 (Moderado)  ·  🌡️ Rocío: 14°       │
│              │    └───┘      │  🌧️ Lluvia: 0.0 mm/h  ·  Hoy: 2.5 mm         │
│  Máx 26°     │    gauge      │                                               │
│  Mín 18°     │   circular    │                              (500px)          │
│              │               │                                               │
│   (200px)    │   (160px)     │                                               │
│              │               │                                               │
├──────────────┴───────────────┼───────────────────────────────────────────────┤
│                              │                                               │
│     PRONÓSTICO 3 DÍAS        │              INTERIOR                         │
│                              │                                               │
│  ┌───────┐ ┌───────┐ ┌───────┐    🏠 SALA                                   │
│  │  HOY  │ │  LUN  │ │  MAR  │                                               │
│  │  ☀️   │ │  ⛅   │ │  🌧️   │    22.3°C    58%    1018 hPa                 │
│  │ 26/18 │ │ 24/17 │ │ 22/16 │                                               │
│  │  0%   │ │  20%  │ │  60%  │    (vs exterior: +0.8°C)                      │
│  └───────┘ └───────┘ └───────┘                                               │
│         (360px)              │                              (restante)       │
│                              │                                               │
├──────────────────────────────┴───────────────────────────────────────────────┤
│ FOOTER (50px)                                                                │
│  ☀️ UV: 3 Mod │ 🌬️ AQI: 42 │ 👁️ Vis: 10km │ 🌅 06:52 │ 🌇 20:14 │ 🌙 82%    │
└──────────────────────────────────────────────────────────────────────────────┘
```

### 5.2 Colores (tema claro)

| Elemento | Hex | Uso |
|----------|-----|-----|
| Fondo | `#F5F5F5` | Background general |
| Cards | `#FFFFFF` | Paneles |
| Bordes | `#E0E0E0` | Separadores |
| Texto principal | `#2C2C2C` | Valores, títulos |
| Texto secundario | `#666666` | Labels |
| Texto sutil | `#999999` | Unidades, timestamps |
| Acento azul | `#2196F3` | Links, selección |
| Verde OK | `#4CAF50` | Online, normal |
| Amarillo warn | `#FF9800` | Advertencia |
| Rojo alert | `#F44336` | Error, alerta |
| Temperatura cálida | `#FF6B6B` | Máximas |
| Temperatura fría | `#64B5F6` | Mínimas |

### 5.3 Tipografía

| Uso | Fuente | Tamaño |
|-----|--------|--------|
| Hora grande | Montserrat Bold | 64px |
| Temperatura principal | Montserrat Bold | 48px |
| Valores medianos | Montserrat | 28px |
| Fecha | Montserrat | 20px |
| Labels | Montserrat | 16px |
| Datos pequeños | Montserrat | 14px |
| Footer | Montserrat | 12px |

### 5.4 Estilos de panel (LVGL)

```c
// Panel blanco con sombra sutil
lv_style_t style_card;
lv_style_init(&style_card);
lv_style_set_bg_color(&style_card, lv_color_hex(0xFFFFFF));
lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
lv_style_set_radius(&style_card, 16);
lv_style_set_border_width(&style_card, 1);
lv_style_set_border_color(&style_card, lv_color_hex(0xE0E0E0));
lv_style_set_shadow_width(&style_card, 12);
lv_style_set_shadow_color(&style_card, lv_color_hex(0x000000));
lv_style_set_shadow_opa(&style_card, LV_OPA_10);
lv_style_set_shadow_ofs_y(&style_card, 3);
lv_style_set_pad_all(&style_card, 12);
```

---

## 6. Funciones Touch

### 6.1 Gestos soportados

| Gesto | Acción |
|-------|--------|
| **Tap** en panel | Expandir detalle (modal) |
| **Tap** en ⚙️ | Abrir configuración |
| **Swipe izquierda** | Ir a pantalla de historial |
| **Swipe derecha** | Volver a dashboard |
| **Long press** en temp | Alternar °C / °F |
| **Long press** en presión | Alternar hPa / mmHg |
| **Tap** en alerta | Ver detalle de la alerta |

### 6.2 Pantallas navegables

```
┌─────────────────┐     swipe     ┌─────────────────┐     swipe     ┌─────────────────┐
│                 │  ◄─────────►  │                 │  ◄─────────►  │                 │
│    DASHBOARD    │               │    HISTORIAL    │               │  CONFIGURACIÓN  │
│   (principal)   │               │   (gráficas)    │               │    (ajustes)    │
│                 │               │                 │               │                 │
└─────────────────┘               └─────────────────┘               └─────────────────┘
```

### 6.3 Modales (tap para expandir)

- **Temperatura** → Gráfica 24h + sensación térmica detallada
- **Presión** → Gráfica 24h + tendencia + pronóstico local
- **Viento** → Rosa de vientos + historial de ráfagas
- **Pronóstico** → Detalle por hora del día seleccionado
- **Alertas** → Lista completa con timestamps

---

## 7. Estación Remota (BME280 como sensor)

### 7.1 Concepto

El display con BME280 puede funcionar como **estación secundaria** que envía
datos al servidor, igual que un WN31 o GW1100:

```
┌─────────────────────────┐
│   Display ESP32-S3      │
│   + BME280              │
│                         │
│  1. Lee BME280 local    │
│  2. POST → servidor     │──────► /data/report/ (protocolo Ecowitt)
│  3. GET ← datos web     │◄────── /api/current
│  4. Muestra todo        │
└─────────────────────────┘
```

### 7.2 Configuración

En `my_config.h`:
```cpp
#define REMOTE_STATION_ENABLED  true
#define REMOTE_STATION_PASSKEY  "display_sala"   // Identificador único
#define REMOTE_STATION_LABEL    "Display Sala"   // Nombre visible
#define REMOTE_STATION_INTERVAL 60               // Segundos entre envíos
```

### 7.3 Registro en el servidor

Para que el servidor reconozca el display como estación secundaria,
agregar en el `.env` del servidor Ecowitt:

```bash
SECONDARY_STATIONS=display_sala:Display Sala
```

O crearlo desde el panel de administración:
- Ir a Admin → Estaciones → + Agregar estación
- Nombre: `display_sala`
- Passkey: (se detectará automáticamente al primer envío)

### 7.4 Datos enviados

El display envía al servidor (formato Ecowitt):
- `tempinf` — Temperatura interior (°F)
- `humidityin` — Humedad interior (%)
- `baromrelin` — Presión relativa (inHg)

---

## 8. Fases de Implementación

### Fase 1: Base funcional (Semana 1)
- [x] Estructura del proyecto
- [x] Copiar drivers LVGL del proyecto anterior
- [x] Cliente API para Ecowitt (`ecowitt_api.h`)
- [x] Adaptar `config.h` para datos de Ecowitt
- [x] Integrar BME280 con estructura `LocalSensorData`
- [x] Implementar envío como estación remota (`postLocalSensorData`)
- [x] Main loop con WiFi, fetch API, lectura BME280, envío remoto
- [ ] Verificar compilación y arranque
- [ ] Probar conexión WiFi + fetch de API

### Fase 2: UI Dashboard (Semana 2)
- [x] Crear `ui_dashboard.h` basado en layout propuesto
- [x] Panel de reloj con NTP
- [x] Panel de temperatura (exterior + máx/mín)
- [x] Panel de humedad con gauge circular
- [x] Panel de condiciones (viento, UV, lluvia)
- [x] Panel de presión con tendencia
- [x] Pronóstico 3 días
- [x] Panel de alertas/tendencias
- [x] Panel interior (BME280 o consola)
- [x] Header con status WiFi y hora
- [x] Footer con UV, sol/luna

### Fase 3: Alertas e Interior (Semana 3)
- [x] Panel de alertas integrado en dashboard
- [x] Integrar `/api/alerts` y mostrar estado
- [x] Panel de interior (BME280 o dato de consola)
- [x] Comparación "vs ayer" desde `/api/compare`
- [x] Footer con UV, sol/luna
- [ ] Crear `ui_alerts.h` — vista expandida de alertas
- [ ] Gráficas de tendencia (sparklines)

### Fase 4: Navegación Touch (Semana 4)
- [x] Implementar swipe entre pantallas
- [x] Pantalla de historial (gráficas 24h)
- [x] Modales expandibles (tap en paneles)
- [x] Pantalla de configuración
- [x] Indicador de página (dots)
- [x] Animaciones de transición
- [ ] Long-press para cambiar unidades
- [ ] Persistencia de preferencias en NVS

### Fase 5: Wizard y pulido (Semana 5)
- [ ] Wizard de configuración inicial
- [ ] Teclado en pantalla para WiFi/URL
- [ ] Persistencia en NVS
- [ ] Animaciones y transiciones
- [ ] Optimización de rendimiento
- [ ] Pruebas de estabilidad (24h+)

---

## 8. Métricas de éxito

| Métrica | Objetivo |
|---------|----------|
| Tiempo de arranque | < 5 segundos |
| Refresh de datos | Cada 60s sin fallas |
| Frame rate UI | > 30 FPS |
| Uso de memoria | < 6MB PSRAM |
| Estabilidad | 7+ días sin reinicio |
| Precisión hora | ±1 segundo (NTP) |

---

## 9. Riesgos y mitigaciones

| Riesgo | Probabilidad | Impacto | Mitigación |
|--------|--------------|---------|------------|
| PSRAM insuficiente | Baja | Alto | Optimizar buffers, reducir fuentes |
| Tearing visual | Media | Medio | Usar SOLO Mode 1 |
| WiFi inestable | Media | Medio | Reconexión automática, caché local |
| API timeout | Baja | Bajo | Timeout 10s, reintentos, indicador |
| Touch no responde | Baja | Alto | Verificar I2C, reset GT911 |

---

## 10. Referencias

- [Waveshare Wiki](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-7B)
- [LVGL 8.3 Docs](https://docs.lvgl.io/8.3/)
- [ESP32-S3 Datasheet](https://www.espressif.com/en/products/socs/esp32-s3)
- [GT911 Touch Controller](https://www.goodix.com/en/product/touch/touch_screen_controller/GT911)
- [API clima.xe1e.net](https://clima.xe1e.net/api/current)
- [Proyecto Waveshare original](https://github.com/XE1E/Waveshare-ESP32-S3-Touch-LCD-7B-weather-station)
- [Proyecto servidor Ecowitt](https://github.com/XE1E/ecowitt-weather-server-xe1e)

---

*Plan creado: 2026-07-13*
*Última actualización: 2026-07-13*
