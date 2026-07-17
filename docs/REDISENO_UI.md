# Rediseño UI - Alineación con Servidor Web

**Fecha inicio:** 2026-07-16
**Objetivo:** Hacer que la pantalla ESP32-S3 tenga coherencia visual con el servidor web clima.xe1e.net

---

## 1. Análisis del Servidor Web

### Stack del servidor
- Frontend: React + Vite + TypeScript + Tailwind CSS
- Iconos: **Meteocons** (basmilius/weather-icons) - SVGs animados
- Efectos: Canvas para lluvia, nieve, niebla, relámpagos

### Tokens de Diseño Extraídos

#### Colores - Modo Oscuro (Principal)
```css
--surface: #0f1a2a       /* Fondo principal */
--ink: #e8edf5           /* Texto primario */
--muted: #8496a6         /* Texto secundario */
--line: rgba(255,255,255,0.14)  /* Bordes */
```

#### Colores - Modo Claro
```css
--surface: #ffffff
--ink: #16232f
--muted: #55677a
--line: rgba(15,30,50,0.16)
```

#### Colores Weather (Tailwind custom)
```css
weather-sun: #FFB800     /* Sol, UV */
weather-rain: #4A90D9    /* Lluvia */
weather-wind: #7BC8A4    /* Viento */
weather-cold: #88C8F7    /* Frío */
weather-hot: #FF6B6B     /* Calor */
```

#### Cards (Glassmorphism)
```css
background: rgba(255,255,255,0.05)  /* bg-white/5 */
border: 1px solid rgba(255,255,255,0.08)
border-radius: 12px (rounded-lg)
backdrop-filter: blur(12px)  /* NO disponible en LVGL */
```

#### AppBar
```css
background: rgba(10,15,26,0.82)
backdrop-filter: blur(10px)
```

#### Sub-cards (boxes internos)
```css
background: rgba(255,255,255,0.05)
border-radius: 8px
padding: 8px
```

---

## 2. Mapeo a LVGL 8.3

### Colores (actualizar en ui_dashboard.h)

| Web Variable | LVGL Actual | LVGL Nuevo |
|--------------|-------------|------------|
| --surface | `0x0B1120` | `0x0F1A2A` |
| --ink | `0xE8EDF5` | `0xE8EDF5` (ok) |
| --muted | `0x8496A6` | `0x8496A6` |
| --line | `0x2A3545` | `rgba(255,255,255,0.14)` → `0x232D3D` |
| weather-sun | `0xF59E0B` | `0xFFB800` |
| weather-rain | `0x60A5FA` | `0x4A90D9` |
| weather-wind | `0x34D399` | `0x7BC8A4` |

### Limitaciones LVGL vs Web

| Característica | Web | LVGL | Solución |
|----------------|-----|------|----------|
| Blur/glassmorphism | ✅ | ❌ | Usar opacidad sin blur |
| SVG animados | ✅ | ❌ | PNG estáticos + animación LVGL |
| Gradientes complejos | ✅ | Parcial | Gradiente 2 colores o sólido |
| Sombras | ✅ | ✅ | Usar `lv_style_set_shadow_*` |
| Transparencias | ✅ | ✅ | Usar `lv_style_set_bg_opa()` |

---

## 3. Iconos

### Situación actual
- Tenemos: Weather Icons font (erikflowers) - estáticos
- Servidor usa: Meteocons (basmilius) - SVGs animados

### Decisión
**Fase 1:** Usar Weather Icons existentes (ya funcionan)
**Fase 2:** Evaluar convertir Meteocons a PNG + animaciones LVGL

### Mapeo de iconos disponibles

| Concepto | Weather Icons (actual) | Meteocons (web) |
|----------|------------------------|-----------------|
| Sol | WI_DAY_SUNNY | clear-day |
| Luna | WI_NIGHT_CLEAR | clear-night |
| Nublado | WI_CLOUDY | overcast-day |
| Lluvia | WI_RAIN | rain |
| Amanecer | WI_SUNRISE | sunrise |
| Atardecer | WI_SUNSET | sunset |
| Fases luna | WI_MOON_* (8) | moon-* (8) |
| Termómetro | WI_THERMOMETER | thermometer |
| Humedad | WI_HUMIDITY | humidity |
| Barómetro | WI_BAROMETER | barometer |
| Viento | WI_STRONG_WIND | windsock |

---

## 4. Plan de Implementación

### Fase 1: Tokens y Estilos Base
- [ ] Actualizar THEME_DARK con colores del servidor
- [ ] Actualizar THEME_LIGHT con colores del servidor
- [ ] Crear estilo para sub-cards (boxes internos)
- [ ] Ajustar sombras para match con web

### Fase 2: Panel SunMoon (Prueba de concepto)
- [ ] Crear nuevo panel SunMoon con diseño web
- [ ] Implementar 3 sub-cards: Amanecer | Atardecer | Luna
- [ ] Agregar "Horas de luz" en footer del panel

### Fase 3: Rediseñar Dashboard Principal
- [ ] Rediseñar header
- [ ] Rediseñar panel principal
- [ ] Rediseñar cards de ubicación (Local, Jardín, Remota)
- [ ] Rediseñar banner de estado

### Fase 4: Pantallas de Detalle
- [ ] Actualizar pantalla detalle Principal
- [ ] Actualizar pantalla detalle Local
- [ ] Actualizar pantalla detalle Jardín
- [ ] Actualizar pantalla detalle Remota

### Fase 5: Iconos (Opcional/Futuro)
- [ ] Evaluar conversión de Meteocons a PNG
- [ ] Implementar animaciones LVGL básicas

---

## 5. Progreso

### 2026-07-16
- [x] Análisis del servidor web completado
- [x] Tokens de diseño extraídos
- [x] Plan creado
- [x] **Fase 1 completada:** Tokens de color actualizados (THEME_DARK, THEME_LIGHT, weather colors)
- [x] **Fase 2 completada:** Estilo sub-cards creado (`style_subcard`, `createSubCard()`)
- [x] **Fase 3 completada:** Dashboard principal rediseñado
  - Cards de ubicación con layout grid 2x2 de sub-cards
  - Banner de estado con badge "En vivo", hora de actualización, URL servidor
  - Contraste mejorado (bg más oscuro, cards más claras)
- [x] **Fase 4 completada:** Pantallas de detalle actualizadas
  - Colores temáticos (respeta modo oscuro/claro)
  - Header con borde inferior sutil
  - Cards con barra de acento superior
  - Botón "Volver" con estilo consistente

### Cambios realizados:
1. `ui_dashboard.h`:
   - THEME_DARK: `bg=#0A1422`, `card=#1E2D40`, `subcard=#2A3A4A`
   - THEME_LIGHT: actualizado para coincidir con servidor
   - Weather colors: `sun=#FFB800`, `rain=#4A90D9`, `wind=#7BC8A4`, `hot=#FF6B6B`, `cold=#88C8F7`
   - Nuevo estilo `style_subcard` con borde sutil
   - Nueva función `createSubCard()`
   - `createLocationCard()` rediseñado con grid 2x2 de sub-cards
   - Banner de estado mejorado con 3 secciones

2. `ui_navigation.h`:
   - Macros DETAIL_* para colores temáticos en pantallas de detalle
   - `createDetailHeader()` con borde inferior
   - `createDetailCard()` con barra de acento superior
   - `createBackButton()` con estilo consistente
   - Todas las pantallas de detalle usan colores temáticos

---

## 6. Referencias

- Servidor web: https://github.com/XE1E/ecowitt-weather-server-xe1e
- Meteocons: https://github.com/basmilius/weather-icons
- Weather Icons: https://erikflowers.github.io/weather-icons/
- LVGL 8.3 docs: https://docs.lvgl.io/8.3/
