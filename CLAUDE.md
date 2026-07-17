# CLAUDE.md

This file provides guidance to Claude Code when working with this repository.

## Project Overview

ESP32-S3 weather display for Ecowitt weather stations. 7" color touchscreen (1024x600) showing real-time data from a self-hosted Ecowitt server via REST API. Uses LVGL 8.3 for graphics.

## Hardware

- **Board**: Waveshare ESP32-S3-Touch-LCD-7B (Type B)
- **Display**: 7" IPS LCD, 1024x600 RGB interface
- **Touch**: GT911 capacitive (I2C)
- **CPU**: ESP32-S3 dual-core @ 240MHz
- **Memory**: 16MB Flash, 8MB PSRAM (OPI mode)
- **Optional**: BME280 for local indoor readings

### Pin Configuration

| Function | GPIO | Notes |
|----------|------|-------|
| I2C SDA | 8 | Shared bus (CH422G, GT911, BME280) |
| I2C SCL | 9 | Shared bus |
| Touch INT | 4 | LOW = touch active |
| Touch RST | CH422G IO1 | Via I2C expander |
| LCD Backlight | CH422G IO2 | Via I2C expander (PWM) |

**IMPORTANT:** I2C is on GPIO 8/9, NOT 19/20 as some docs suggest!

## Architecture

### Data Source

This display connects to a self-hosted Ecowitt weather server (e.g., clima.xe1e.net) instead of commercial weather APIs.

### Primary Endpoint

```
GET /api/display   - Optimized endpoint combining ALL data in one call
```

Response includes:
- `timezone_offset`: Server timezone (e.g., -6 for Mexico)
- `current`: Temperature, humidity, wind, rain, UV, pressure
- `stats`: Daily min/max per sensor
- `compare`: Differences vs yesterday
- `almanac`: Sunrise, sunset, moon phase
- `forecast`: Barometric prediction
- `airquality`: AQI (if configured)
- `stations`: Secondary stations (ch1/WN31, gw1100)

### Other Endpoints

```
GET /api/alerts               - Active weather alerts (separate call)
GET /api/history              - Historical data (pending)
GET /api/climate/records      - All-time records (pending)
```

### Main Flow

```
setup() → Init PSRAM → Init display/touch → Load WiFi from NVS
       → Try 3 WiFi networks → Create LVGL UI → Main loop

loop()  → lv_timer_handler() → Every 60s: fetchAll() → Sync time → Update UI
```

## Build Configuration

### PlatformIO (Recommended)

```bash
pio run -t upload        # Build and upload
pio run -t monitor       # Serial monitor
```

Key settings in `platformio.ini`:
- `board_build.arduino.memory_type = qio_opi` (8MB PSRAM)
- `board_build.partitions = default_16MB.csv`
- LVGL 8.3.x, GFX Library 1.3.7, ArduinoJson 7.x

### Arduino IDE (Alternative)

- Board: ESP32S3 Dev Module
- USB CDC On Boot: Enable
- Flash Size: 16MB (128Mb)
- Flash Mode: QIO 80MHz
- PSRAM: OPI PSRAM (required!)
- Partition Scheme: 16M Flash (3M APP/9.9MB FATFS)

### Critical: Anti-Tearing Mode

```c
// lvgl_port.h - ONLY use Mode 1
#define LVGL_PORT_AVOID_TEAR_MODE  (1)
// Mode 2 = crash, Mode 3 = artifacts
```

### Libraries

| Library | Version | Notes |
|---------|---------|-------|
| lvgl | 8.3.x | NOT 9.x |
| GFX Library for Arduino | 1.3.7 | Exact version |
| ArduinoJson | 7.x | JSON parsing |
| GT911 Lite | 1.0.x | Touch |
| Adafruit BME280 | latest | Optional |

## Code Style

- Arduino/C++ style
- LVGL 8.3 API (not 9.x)
- Use `lv_timer_create()` for periodic tasks, never `delay()`
- Store config in NVS via `Preferences.h`
- Use HTTPS for API calls
- Spanish for UI labels, English for code/comments

## UI Guidelines

### Colors (Light Theme)

| Element | Hex |
|---------|-----|
| Background | #F5F5F5 |
| Card/Panel | #FFFFFF |
| Border | #E0E0E0 |
| Text Primary | #2C2C2C |
| Text Secondary | #666666 |
| Accent Blue | #2196F3 |
| Accent Green | #4CAF50 |
| Accent Red | #F44336 |
| Alert/Warning | #FF9800 |

### Typography

Available font sizes: 12, 14, 16, 18, 20, 24, 28, 36, 40, 48 px

- Temperature: 48-64px bold
- Clock: 48px bold
- Values: 24-28px
- Labels: 16px
- Small text: 12-14px

### Current Layout (1024x600)

```
┌─────────────────────────────────────────────────────┐
│ HEADER: Station name, Time, Status, Settings       │
├─────────┬─────────┬─────────┬─────────┬────────────┤
│  TEMP   │ HUMIDITY│ PRESSURE│  WIND   │  SUN/MOON  │
│  23.4°C │   65%   │ 1013hPa │ 12km/h  │  ☀ 06:07  │
│  ↑25↓18 │         │ →stable │ NO 303° │  ☽ 20:18  │
├─────────┴─────────┴─────────┴─────────┴────────────┤
│ SENSORS: Interior | Jardin (WN31) | Remoto (GW1100)│
└─────────────────────────────────────────────────────┘
```

## File Structure

| File | Purpose |
|------|---------|
| `*.ino` | Main entry point |
| `platformio.ini` | PlatformIO config |
| `config.h` | Data structures |
| `my_config.h` | User config (WiFi, API URL) - gitignored |
| `ecowitt_api.h` | REST API client with `fetchAll()` |
| `time_sync.h` | Server time synchronization |
| `preferences_manager.h` | NVS persistence (3 WiFi networks) |
| `display.h` | Display + CH422G backlight PWM |
| `touch.h` | GT911 touch driver |
| `ui.h` | Main UI controller |
| `ui_dashboard.h` | Dashboard screen |
| `ui_settings.h` | Settings screen with brightness slider |
| `lv_conf.h` | LVGL config |

## Key Functions

### ecowitt_api.h
```cpp
// Single call to get ALL data (primary method)
bool fetchAll(WeatherData&, CompareData&, AlmanacData&, 
              RemoteSensorData&, RemoteGatewayData&, int* tzOffset);

bool fetchAlerts(AlertData&);  // Separate call for alerts
```

### time_sync.h
```cpp
// Sync system clock from server timestamp + timezone
bool syncTimeFromServer(const char* isoTimestamp, int timezoneOffsetHours);
```

### preferences_manager.h
```cpp
// Supports 3 WiFi networks with automatic fallback
char wifi_ssid1[33], wifi_pass1[65];
char wifi_ssid2[33], wifi_pass2[65];
char wifi_ssid3[33], wifi_pass3[65];
```

## Common Issues

| Problem | Solution |
|---------|----------|
| Display tearing/jumping | Reduce pixel clock to 14-15MHz in display.h |
| Touch not working | I2C on GPIO 8/9 (not 19/20!), check CH422G init |
| Ghost touches | Filter with INT pin (GPIO 4), only read when LOW |
| Swipe not working | Use LV_EVENT_GESTURE, add LV_OBJ_FLAG_CLICKABLE |
| Accents not showing | Enable extended fonts in lv_conf.h |
| Black screen + reboot | Use auto_flush=false in Arduino_RGB_Display |
| Out of memory | Use PSRAM buffers (ps_malloc), reduce chart points |
| Brightness not working | Use `ch422g_set_backlight_pwm()`, value is inverted |
| Time showing UTC | Pass `timezone_offset` from server to `syncTimeFromServer()` |

## Display Stability

```c
// display.h - Stable settings
Arduino_RGB_Display(..., false);  // auto_flush = false
prefer_speed = 15000000;          // 15MHz pixel clock (not 30MHz!)
buf_size = SCREEN_WIDTH * 200;    // 200 lines buffer in PSRAM
```

## Documentation

- `docs/PLAN_FUNCIONALIDADES.md` - Feature roadmap and phases
- `docs/API.md` - Server API reference
