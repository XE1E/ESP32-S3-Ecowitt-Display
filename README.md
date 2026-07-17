# ESP32-S3 Ecowitt Display

Display tactil de 7" para estacion meteorologica Ecowitt. Muestra en tiempo real los datos de tu propia estacion via API REST.

## Caracteristicas

- **Pantalla tactil 7" IPS** a color (1024x600)
- **Datos en tiempo real** de tu estacion Ecowitt
- **Dashboard con cards** - Temperatura, Humedad, Viento, Presion, Sol/Luna
- **Multi-estacion** - Principal + Jardin (WN31) + Remoto (GW1100)
- **3 redes WiFi** - Fallback automatico entre redes configuradas
- **Sincronizacion hora** - Desde el servidor (sin NTP)
- **Control de brillo** - Slider PWM en configuracion
- **UI con LVGL 8.3** - Interfaz moderna y fluida
- **Configuracion tactil** - WiFi y ajustes en pantalla

## Hardware

### Waveshare ESP32-S3-Touch-LCD-7B

| Especificacion | Valor |
|----------------|-------|
| Display | 7" IPS LCD, 1024x600 pixeles |
| Touch | GT911 capacitivo multi-touch |
| CPU | ESP32-S3 dual-core @ 240MHz |
| Flash | 16MB |
| PSRAM | 8MB (OPI mode) |
| WiFi | 802.11 b/g/n 2.4GHz |
| USB | Type-C (CDC + JTAG) |

### Pines importantes

| Funcion | GPIO | Notas |
|---------|------|-------|
| I2C SDA | 8 | Bus compartido (touch, BME280) |
| I2C SCL | 9 | Bus compartido |
| Touch INT | 4 | LOW = touch activo |
| Backlight | CH422G | Control PWM via I2C |

### Sensor interior (opcional)

| Sensor | Conexion |
|--------|----------|
| BME280 | I2C (GPIO 8/9) |
| Direccion | 0x76 o 0x77 |

## Arquitectura

```
┌─────────────────────┐         ┌─────────────────────┐
│  Display 7" tactil  │  WiFi   │  Servidor Ecowitt   │
│  ESP32-S3 + LVGL    │ <-----> │  clima.xe1e.net     │
│                     │  HTTPS  │  /api/display       │
└─────────────────────┘         └─────────────────────┘
```

El display usa un **endpoint optimizado** (`/api/display`) que combina todos los datos en una sola llamada HTTP.

## Configuracion PlatformIO (Recomendado)

### Instalacion

1. Instala [PlatformIO](https://platformio.org/install)
2. Clona este repositorio
3. Copia `my_config.h.template` a `my_config.h`
4. Edita `my_config.h` con tu WiFi y URL de API
5. Compila y sube:

```bash
pio run -t upload
```

### platformio.ini

```ini
[env:esp32s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

; Flash y PSRAM para Waveshare 16MB/8MB
board_build.flash_mode = qio
board_build.f_flash = 80000000L
board_upload.flash_size = 16MB
board_build.partitions = default_16MB.csv
board_build.arduino.memory_type = qio_opi
board_build.psram_type = opi

build_flags =
    -DBOARD_HAS_PSRAM
    -DCONFIG_SPIRAM_MODE_OCT=1
    -DLV_CONF_INCLUDE_SIMPLE=1

monitor_speed = 115200
upload_speed = 921600

lib_deps =
    lvgl/lvgl@~8.3.0
    moononournation/GFX Library for Arduino@1.3.7
    bblanchon/ArduinoJson@^7.0.0
    adafruit/Adafruit BME280 Library@^2.2.0
    tonywestonuk/GT911 Lite@^1.0.0
```

## Configuracion Arduino IDE (Alternativo)

| Setting | Valor |
|---------|-------|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | Enable |
| Flash Size | 16MB (128Mb) |
| Flash Mode | QIO 80MHz |
| PSRAM | OPI PSRAM |
| Partition Scheme | 16M Flash (3M APP/9.9MB FATFS) |

### Librerias requeridas

| Libreria | Version | Notas |
|----------|---------|-------|
| esp32 (Espressif) | 2.0.17+ | Board Manager (NO usar 3.x) |
| lvgl | 8.3.x | **NO usar 9.x** |
| GFX Library for Arduino | 1.3.7 | **Esta version exacta** |
| ArduinoJson | 6.x o 7.x | Parser JSON |
| Adafruit BME280 | Latest | Sensor interior (opcional) |
| GT911 Lite | 1.0.2 | Touch controller |

### Instalacion Arduino

1. Abre `ESP32-S3-Ecowitt-Display.ino` en Arduino IDE
2. Copia `my_config.h.template` a `my_config.h`
3. Edita `my_config.h` con tu WiFi y URL de API
4. Copia `lv_conf.h` a tu carpeta `Arduino/libraries/`
5. Compila y sube al ESP32-S3

## Configuracion inicial

Al primer arranque aparece el wizard de configuracion:
1. Selecciona tu red WiFi (soporta hasta 3 redes)
2. Ingresa la contrasena
3. Configura la URL de tu servidor Ecowitt
4. Ajusta el brillo de pantalla
5. El display reinicia y comienza a mostrar datos

## API del Servidor

El display consume el endpoint optimizado `/api/display`:

```json
{
  "timezone_offset": -6,
  "current": { "temperature_outdoor": 25.3, ... },
  "stats": { "temperature_outdoor": { "min": 18, "max": 28 } },
  "compare": { "temp_diff": 2.3 },
  "almanac": { "sunrise": "06:07", "sunset": "20:18" },
  "stations": { "ch1": {...}, "gw1100": {...} }
}
```

Ver [docs/API.md](docs/API.md) para documentacion completa.

## Estructura del proyecto

```
ESP32-S3-Ecowitt-Display/
├── ESP32-S3-Ecowitt-Display.ino   # Archivo principal
├── platformio.ini                  # Config PlatformIO
├── config.h                        # Estructuras de datos
├── my_config.h                     # Tu configuracion (gitignore)
├── my_config.h.template            # Plantilla de configuracion
│
├── display.h                       # Inicializacion display RGB
├── touch.h                         # Driver touch GT911
├── ecowitt_api.h                   # Cliente API REST
├── time_sync.h                     # Sincronizacion hora con servidor
├── preferences_manager.h           # Persistencia NVS (WiFi, config)
├── bme280_sensor.h                 # Sensor interior
│
├── ui.h                            # Controlador UI principal
├── ui_dashboard.h                  # Pantalla principal (cards)
├── ui_settings.h                   # Pantalla configuracion
├── ui_components.h                 # Componentes reutilizables
│
├── lv_conf.h                       # Config LVGL
├── fonts/                          # Fuentes custom (Weather Icons)
│
├── docs/
│   ├── README.md                   # Indice de documentacion
│   ├── PLAN_FUNCIONALIDADES.md     # Plan de desarrollo
│   ├── API.md                      # Referencia API servidor
│   └── archivo/                    # Docs obsoletos
│
└── unused_waveshare/               # Codigo original Waveshare (referencia)
```

## Notas tecnicas

| Parametro | Valor |
|-----------|-------|
| Pixel clock | 15 MHz (estable, sin tearing) |
| Buffer LVGL | 200 lineas en PSRAM |
| I2C | GPIO 8 (SDA), GPIO 9 (SCL) |
| Touch INT | GPIO 4 (LOW = touch activo) |
| Fuentes | Montserrat 12, 14, 16, 18, 20, 24, 28, 36, 40, 48 px |

## Documentacion

- [Plan de Funcionalidades](docs/PLAN_FUNCIONALIDADES.md) - Roadmap y fases
- [API Reference](docs/API.md) - Documentacion del servidor

## Licencia

MIT

---

Proyecto complementario de [Ecowitt Weather Server XE1E](https://github.com/XE1E/ecowitt-weather-server-xe1e)
