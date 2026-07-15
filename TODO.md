# TODO - ESP32-S3 Ecowitt Display

## Problemas Resueltos
- [x] Touch no funcionaba - Pines I2C corregidos (GPIO 8/9)
- [x] Display tearing/brincos - Pixel clock reducido a 15MHz
- [x] Toques fantasma - Filtrado con pin INT del GT911
- [x] Swipe no funcionaba - Usar LV_EVENT_GESTURE de LVGL
- [x] Boton settings sin callback - Agregado en initNavigation()

## Pendientes Inmediatos

### UI/Display
- [ ] **Caracteres con acento** - Faltan glifos en fuente (U+F3, U+E1, U+ED, etc.)
- [ ] **Emojis no se muestran** - Necesitan fuente con soporte Unicode extendido
- [ ] **Tema oscuro** - Crear paleta de colores dark mode
- [ ] **Velocidad de refresh** - Optimizar si es posible sin tearing

### Touch/Navegacion
- [ ] **Swipe en pantallas secundarias** - Verificar que funcione en History y Settings
- [ ] **Long press** - Implementar para acciones secundarias
- [ ] **Indicador de pagina** - Los dots no se actualizan visualmente

### Datos
- [ ] **Graficas con datos reales** - Conectar a /api/history
- [ ] **Actualizar labels correctamente** - Algunos muestran valores hardcoded
- [ ] **Cache offline** - Guardar ultimo dato en NVS

### Configuracion
- [ ] **Wizard mejorado** - Agregar mas pasos (servidor, alertas, etc.)
- [ ] **Slider brillo funcional** - Conectar a CH422G PWM
- [ ] **Guardar preferencias** - Unidades, tema, idioma

## Por Implementar (Fase 2)

### Nuevas Pantallas
- [ ] Pantalla de Alertas (lista de alertas activas)
- [ ] Pantalla de Pronostico (5-7 dias)
- [ ] Pantalla de Almanaque (sol, luna, planetas)
- [ ] Pantalla de Estaciones (multi-station)
- [ ] Pantalla de Viento (rosa de vientos)
- [ ] Pantalla de Lluvia (acumulados)
- [ ] Pantalla de Calidad del Aire
- [ ] Pantalla de Admin del Servidor

### Widgets Avanzados
- [ ] Medidor circular de presion (lv_meter)
- [ ] Arco de UV (lv_arc)
- [ ] Indicador de fase lunar grafico
- [ ] Grafica de temperatura con gradiente
- [ ] Animacion de lluvia/sol/nubes

### Conectividad
- [ ] WebSocket o polling mas eficiente
- [ ] MQTT directo (sin pasar por servidor)
- [ ] BLE para configuracion desde movil
- [ ] OTA updates desde servidor

## Bugs Conocidos
- Warnings LVGL: "glyph dsc. not found" para caracteres especiales
- El reloj puede desfasarse si NTP falla

## Notas Tecnicas
- Pixel clock estable: 14-15 MHz (mas rapido causa tearing)
- Buffer LVGL: 200 lineas en PSRAM
- I2C: GPIO 8 (SDA), GPIO 9 (SCL) - NO 19/20
- Touch INT: GPIO 4 (LOW = touch activo)
