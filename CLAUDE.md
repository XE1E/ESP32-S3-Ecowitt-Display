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

| Function | GPIO |
|----------|------|
| Touch SDA | 19 |
| Touch SCL | 20 |
| Touch RST | 38 |
| Touch INT | 18 |
| BME280 SDA | 19 (shared) |
| BME280 SCL | 20 (shared) |
| LCD Backlight | 2 |

## Architecture

### Data Source

This display connects to a self-hosted Ecowitt weather server (e.g., clima.xe1e.net) instead of commercial weather APIs. Benefits:

- No API limits or costs
- Hyper-local real data from own station
- Full control over data and features
- Can show alerts, comparisons, etc.

### Key Endpoints

```
GET /api/current      - Current weather data
GET /api/stats/daily  - Day statistics (max/min/totals)
GET /api/compare      - Comparison vs yesterday
GET /api/alerts       - Active alerts
GET /api/forecast     - Weather forecast (Open-Meteo proxy)
GET /api/almanac      - Sun, moon, planets
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
| Black screen + reboot loop | Use LVGL_PORT_AVOID_TEAR_MODE = 1 |
| Touch not working | Check I2C pins 19/20 |
| Out of memory | Reduce chart points, check PSRAM enabled |
| API timeout | Check WiFi, increase timeout |
| Font not found | Enable font in lv_conf.h |
