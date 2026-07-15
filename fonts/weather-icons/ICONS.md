# Weather Icons - Seleccion para ESP32 Display

Fuente: [erikflowers/weather-icons](https://github.com/erikflowers/weather-icons)
Licencia: SIL OFL 1.1

## Iconos Seleccionados

### Condiciones del Clima

| Icono | Nombre | Unicode | Uso |
|-------|--------|---------|-----|
| ☀ | wi-day-sunny | F00D | Dia soleado |
| 🌙 | wi-night-clear | F02E | Noche despejada |
| ⛅ | wi-day-cloudy | F002 | Parcialmente nublado (dia) |
| 🌥 | wi-night-alt-cloudy | F086 | Parcialmente nublado (noche) |
| ☁ | wi-cloudy | F013 | Nublado |
| 🌧 | wi-rain | F019 | Lluvia |
| ⛈ | wi-thunderstorm | F01E | Tormenta |
| 🌨 | wi-snow | F01B | Nieve |
| 🌫 | wi-fog | F014 | Niebla |
| 💨 | wi-strong-wind | F050 | Viento fuerte |
| 🌦 | wi-day-rain | F008 | Lluvia con sol |
| 🌩 | wi-day-thunderstorm | F010 | Tormenta diurna |

### Indicadores

| Icono | Nombre | Unicode | Uso |
|-------|--------|---------|-----|
| 🌡 | wi-thermometer | F055 | Temperatura |
| 💧 | wi-humidity | F07A | Humedad |
| 📊 | wi-barometer | F079 | Presion |
| 🌅 | wi-sunrise | F051 | Amanecer |
| 🌇 | wi-sunset | F052 | Atardecer |
| ☂ | wi-umbrella | F084 | Probabilidad lluvia |
| 🔆 | wi-hot | F072 | Calor |
| ❄ | wi-snowflake-cold | F076 | Frio |

### Fases Lunares

| Icono | Nombre | Unicode | Fase |
|-------|--------|---------|------|
| 🌑 | wi-moon-new | F095 | Nueva |
| 🌒 | wi-moon-waxing-crescent-3 | F0D2 | Creciente |
| 🌓 | wi-moon-first-quarter | F09C | Cuarto creciente |
| 🌔 | wi-moon-waxing-gibbous-3 | F0D5 | Gibosa creciente |
| 🌕 | wi-moon-full | F0A3 | Llena |
| 🌖 | wi-moon-waning-gibbous-3 | F0D8 | Gibosa menguante |
| 🌗 | wi-moon-third-quarter | F0AA | Cuarto menguante |
| 🌘 | wi-moon-waning-crescent-3 | F0DB | Menguante |

### Direcciones del Viento

| Icono | Nombre | Unicode | Direccion |
|-------|--------|---------|-----------|
| ↑ | wi-direction-up | F058 | N |
| ↗ | wi-direction-up-right | F057 | NE |
| → | wi-direction-right | F04D | E |
| ↘ | wi-direction-down-right | F044 | SE |
| ↓ | wi-direction-down | F044 | S |
| ↙ | wi-direction-down-left | F043 | SO |
| ← | wi-direction-left | F048 | O |
| ↖ | wi-direction-up-left | F056 | NO |

## Conversion a LVGL

Para convertir la fuente TTF a formato LVGL, usar lv_font_conv:

```bash
# Instalar
npm install -g lv_font_conv

# Convertir (solo iconos seleccionados, 48px)
lv_font_conv --font weathericons-regular-webfont.ttf \
  --symbols 0xF00D,0xF02E,0xF002,0xF086,0xF013,0xF019,0xF01E,0xF01B,0xF014,0xF050,0xF008,0xF010,0xF055,0xF07A,0xF079,0xF051,0xF052,0xF084,0xF072,0xF076,0xF095,0xF0D2,0xF09C,0xF0D5,0xF0A3,0xF0D8,0xF0AA,0xF0DB \
  --size 48 \
  --format lvgl \
  --bpp 4 \
  -o weather_icons_48.c
```

## Uso en LVGL

```c
// Declarar fuente
LV_FONT_DECLARE(weather_icons_48);

// Crear label con icono
lv_obj_t *icon = lv_label_create(parent);
lv_obj_set_style_text_font(icon, &weather_icons_48, 0);
lv_label_set_text(icon, "\xEF\x80\x8D");  // F00D = dia soleado (UTF-8)

// O usar define
#define ICON_SUNNY      "\xEF\x80\x8D"
#define ICON_CLOUDY     "\xEF\x80\x93"
#define ICON_RAIN       "\xEF\x80\x99"
#define ICON_SNOW       "\xEF\x80\x9B"
#define ICON_THUNDER    "\xEF\x80\x9E"
#define ICON_FOG        "\xEF\x80\x94"
#define ICON_WIND       "\xEF\x81\x90"

lv_label_set_text(icon, ICON_SUNNY);
```

## Notas

- Los codigos Unicode estan en el rango Private Use Area (U+F000 - U+F0EB)
- En UTF-8, U+F00D se codifica como `\xEF\x80\x8D`
- Tamanos recomendados: 32px, 48px, 64px
