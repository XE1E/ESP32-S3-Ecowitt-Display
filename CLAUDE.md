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

This display connects to a self-hosted Ecowitt weather server (e.g., clima.xe1e.net) instead of commercial weather APIs. Benefits:

- No API limits or costs
- Hyper-local real data from own station
- Full control over data and features
- Can show alerts, comparisons, etc.

### Key Endpoints (ecowitt-weather-server-xe1e)

```
# Core Data
GET /api/current              - Current weather readings
GET /api/history              - Historical data (time range)
GET /api/stats/daily          - Day min/max/avg statistics
GET /api/compare              - Comparison vs yesterday

# Climatology
GET /api/climate/daily        - Daily summaries
GET /api/climate/records      - All-time and monthly records
GET /api/wind/rose            - Wind direction distribution

# External Data
GET /api/alerts               - Active weather alerts
GET /api/forecast/local       - Barometric forecast
GET /api/almanac              - Sun, moon, planets
GET /api/airquality           - AQI from WAQI
GET /api/metar?station=MMMX   - Aviation weather
GET /api/satellite            - NASA satellite imagery

# Multi-Station
GET /api/stations             - List all stations
GET /api/current?station=X    - Data from station X

# Admin (requires Bearer token)
POST /api/admin/login         - Get auth token
GET  /api/admin/settings      - Server configuration
POST /api/admin/settings      - Update configuration
GET  /api/admin/status        - System status
```

### Main Flow

```
setup() → Init PSRAM → Init display/touch → Load WiFi from NVS
       → Connect WiFi → Create LVGL UI → Main loop

loop()  → lv_timer_handler() → Every 60s: fetch API → Update UI
```

## Build Configuration

### Arduino IDE Settings

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

- esp32 by Espressif: 2.0.17+ (not 3.x)
- lvgl: 8.3.x (not 9.x)
- GFX Library for Arduino: 1.3.7 exactly
- ArduinoJson: 6.x or 7.x
- Adafruit BME280: latest (optional)

## Code Style

- Arduino/C++ style
- LVGL 8.3 API (not 9.x)
- Use `lv_timer_create()` for periodic tasks, never `delay()`
- Store config in NVS via `Preferences.h`
- Use HTTPS for API calls

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

- Temperature: 48-64px bold
- Clock: 48px bold
- Values: 24-28px
- Labels: 16px
- Small text: 12-14px

### Layout (1024x600)

```
┌─────────────────────────────────────────────────────┐
│ HEADER: Status, Time, Settings button      (full)  │
├────────────┬────────────┬───────────────────────────┤
│  CLOCK     │  PRESSURE  │  TRENDS / ALERTS          │
│  (200px)   │  (180px)   │  (remaining)              │
├────────────┼────────────┼───────────────────────────┤
│   TEMP     │  HUMIDITY  │  CONDITIONS / WEATHER     │
│  (200px)   │  (180px)   │  (remaining)              │
├────────────┴────────────┼───────────────────────────┤
│  FORECAST 3 DAYS        │  INDOOR (BME280)          │
├─────────────────────────┴───────────────────────────┤
│ FOOTER: UV, AQI, Sunrise/Sunset, Moon      (full)  │
└─────────────────────────────────────────────────────┘
```

## File Structure

| File | Purpose |
|------|---------|
| `*.ino` | Main entry point |
| `config.h` | Data structures |
| `my_config.h` | User config (WiFi, API URL) - gitignored |
| `ecowitt_api.h` | REST API client |
| `ui.h` | Main UI controller |
| `ui_dashboard.h` | Dashboard screen |
| `ui_settings.h` | Settings screen |
| `lvgl_port.h` | LVGL + display init |
| `lv_conf.h` | LVGL config (copy to libraries/) |

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
| Serial not visible | Use UART mode (CDCOnBoot=default) not USB CDC |

## Display Stability

```c
// display.h - Stable settings
Arduino_RGB_Display(..., false);  // auto_flush = false
prefer_speed = 15000000;          // 15MHz pixel clock (not 30MHz!)
buf_size = SCREEN_WIDTH * 200;    // 200 lines buffer in PSRAM
```
