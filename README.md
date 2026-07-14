# ESP32-S3 Ecowitt Display

Display táctil de 7" para estación meteorológica Ecowitt. Muestra en tiempo real los datos de tu propia estación vía API REST.

**Hardware:** Waveshare ESP32-S3-Touch-LCD-7B (1024×600)

## Características

- **Pantalla táctil 7" IPS** a color (1024×600)
- **Datos en tiempo real** de tu estación Ecowitt
- **Interior + Exterior** — BME280 local + sensores de la estación
- **Alertas activas** — muestra si hay alertas disparadas
- **Tendencias** — comparación vs hace 3 horas
- **Pronóstico** — próximos días (Open-Meteo)
- **Almanaque** — sol, luna, planetas
- **UI con LVGL** — interfaz moderna y fluida
- **Configuración táctil** — WiFi y ajustes en pantalla

## Hardware requerido

| Componente | Modelo |
|------------|--------|
| Board + Display | Waveshare ESP32-S3-Touch-LCD-7B (Type B) |
| Sensor interior (opcional) | BME280 (I2C) |

## Conexión

El display se conecta a tu servidor Ecowitt vía WiFi:

```
┌─────────────────────┐         ┌─────────────────────┐
│  Display 7" táctil  │  WiFi   │  Servidor Ecowitt   │
│  ESP32-S3 + LVGL    │ ◄─────► │  clima.xe1e.net     │
│                     │  HTTPS  │  /api/current       │
└─────────────────────┘         └─────────────────────┘
```

## API Endpoints usados

```
GET /api/current      → Datos actuales (temp, humedad, viento, lluvia, UV...)
GET /api/stats/daily  → Estadísticas del día (máx, mín, totales)
GET /api/compare      → Comparación vs ayer
GET /api/alerts       → Alertas activas
GET /api/forecast     → Pronóstico (Open-Meteo)
GET /api/almanac      → Sol, luna, planetas
```

## Configuración Arduino IDE

| Setting | Valor |
|---------|-------|
| Board | ESP32S3 Dev Module |
| USB CDC On Boot | Enable |
| Flash Size | 16MB (128Mb) |
| Flash Mode | QIO 80MHz |
| PSRAM | OPI PSRAM |
| Partition Scheme | 16M Flash (3M APP/9.9MB FATFS) |

## Librerías requeridas

| Librería | Versión | Notas |
|----------|---------|-------|
| esp32 (Espressif) | 2.0.17+ | Board Manager |
| lvgl | 8.3.x | **No usar 9.x** |
| GFX Library for Arduino | 1.3.7 | **Esta versión exacta** |
| ArduinoJson | 6.x o 7.x | Parser JSON |
| Adafruit BME280 | Latest | Sensor interior (opcional) |

## Instalación

1. Clona este repositorio
2. Abre `ESP32-S3-Ecowitt-Display.ino` en Arduino IDE
3. Copia `my_config.h.template` a `my_config.h`
4. Edita `my_config.h` con tu WiFi y URL de API
5. Copia `lv_conf.h` a tu carpeta `Arduino/libraries/`
6. Compila y sube al ESP32-S3

## Configuración inicial

Al primer arranque:
1. Conecta al WiFi "EcowittDisplay-Setup"
2. Abre http://192.168.4.1 en tu navegador
3. Configura WiFi y URL de tu servidor Ecowitt
4. El display reinicia y comienza a mostrar datos

## Estructura del proyecto

```
├── ESP32-S3-Ecowitt-Display.ino   # Archivo principal
├── config.h                        # Estructuras de configuración
├── my_config.h                     # Tu configuración (gitignore)
├── ecowitt_api.h                   # Cliente API Ecowitt
├── ui.h                            # Controlador principal UI
├── ui_dashboard.h                  # Pantalla principal
├── ui_settings.h                   # Pantalla de ajustes
├── lvgl_port.h                     # Configuración LVGL/display
├── lv_conf.h                       # Configuración LVGL (copiar a libraries/)
└── icons/                          # Iconos del clima
```

## Licencia

MIT

---

Proyecto complementario de [Ecowitt Weather Server XE1E](https://github.com/XE1E/ecowitt-weather-server-xe1e)
