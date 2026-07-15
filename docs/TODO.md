# TODO - ESP32-S3 Ecowitt Display

## Estado: 2026-07-14

## Completado

### Hardware/Display
- [x] Display RGB funcionando (1024x600)
- [x] Touch GT911 funcionando (I2C GPIO 8/9)
- [x] Pixel clock estable (15MHz, sin tearing)
- [x] Filtrado de toques fantasma (pin INT)
- [x] Backlight via CH422G

### UI/Layout
- [x] Layout 6 cards grandes (2 filas x 3 columnas)
- [x] Header estilo servidor (nombre, fecha, hora, controles)
- [x] Tema oscuro con colores del servidor
- [x] Fuentes grandes (48px para valores principales)
- [x] Indicadores de color por categoria
- [x] Eliminados acentos y emojis problematicos

### Navegacion
- [x] Swipe entre pantallas (LV_EVENT_GESTURE)
- [x] Boton settings funcional
- [x] Toggle tema claro/oscuro
- [x] Toggle unidades C/F

### Persistencia
- [x] Preferencias en NVS (tema, unidades)
- [x] Wizard de configuracion WiFi
- [x] Estructura de preferences_manager.h

## Pendiente Inmediato

### UI/Display
- [ ] Conectar datos reales del API a todos los paneles
- [ ] Iconos del clima (texto o graficos)
- [ ] Animaciones de iconos del clima

### Pantallas Adicionales
- [ ] Pantalla de Pronostico detallado (5-7 dias)
- [ ] Pantalla de Graficas/Historia
- [ ] Pantalla de Interior (BME280)
- [ ] Pantalla de Configuracion mejorada

### Funcionalidad
- [ ] Long-press para acciones secundarias
- [ ] Slider de brillo funcional
- [ ] Cache offline de ultimo dato
- [ ] Reconexion WiFi automatica

## Fase 2 (Futuro)

### Nuevas Pantallas
- [ ] Pantalla de Alertas (lista completa)
- [ ] Pantalla de Almanaque (sol, luna, planetas)
- [ ] Pantalla de Viento (rosa de vientos)
- [ ] Pantalla de Lluvia (acumulados detallados)
- [ ] Pantalla de Calidad del Aire
- [ ] Pantalla de Admin del Servidor

### Widgets Avanzados
- [ ] Graficas con lv_chart
- [ ] Medidor circular de presion (lv_meter)
- [ ] Rosa de vientos animada
- [ ] Indicador de fase lunar grafico

### Conectividad
- [ ] MQTT directo
- [ ] BLE para configuracion
- [ ] OTA updates

## Notas Tecnicas

- Pixel clock: 15 MHz (estable)
- Buffer LVGL: 200 lineas en PSRAM
- I2C: GPIO 8 (SDA), GPIO 9 (SCL)
- Touch INT: GPIO 4 (LOW = touch activo)
- Fuentes disponibles: 12, 14, 16, 18, 20, 24, 28, 36, 40, 48

---
*Actualizado: 2026-07-14*
