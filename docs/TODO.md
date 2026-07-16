# TODO - ESP32-S3 Ecowitt Display

## Estado: 2026-07-15

## Arquitectura de Pantallas (En Discusion)

### Dashboard (Pantalla Principal)
Vista resumen del sistema completo - un vistazo rapido a todo.

| Ubicacion | Sensores | Datos |
|-----------|----------|-------|
| **Principal** | Estacion Ecowitt | Temp (max/min), Humedad, Presion, Viento + Rafagas |
| **Local** | BME280 indoor | Temp (max/min), Humedad, Bateria |
| **Jardin** | HN31 | Temp (max/min), Humedad, Bateria |
| **Remoto** | GW110 | Temp (max/min), Humedad, Presion |

Adicional:
- Estado del servidor (online, ultima actualizacion)
- Iconos del clima
- Tap en card → abre pantalla de detalle

### Pantallas de Detalle (4 pantallas)
Una pantalla por ubicacion con:
- Todas las lecturas disponibles del sensor/estacion
- Historial (graficas)
- Estadisticas (min/max/avg diario, records)
- Estilo similar al servidor web

**Principal** tendra mas info (UV, lluvia, solar, etc.)

### Navegacion
- Swipe horizontal entre pantallas
- Tap en card de Dashboard → detalle de esa ubicacion
- Boton/gesto para volver al Dashboard

---

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

### Iconos del Clima
- [x] Definir set de iconos (Weather Icons font ya convertido)
- [x] Mapear condiciones del API a iconos (derivado de sensores)
- [x] Implementar en Dashboard
- [x] Variantes dia/noche
- [x] Iconos agregados: termometro, humedad, barometro, viento, direccion, lluvia, sol/luna

### Rediseno Dashboard
- [x] Layout 4 cards (Principal, Local, Jardin, Remoto)
- [x] Card Principal: temp, humedad, presion, viento+rafagas, max/min
- [x] Card Local: temp, humedad, bateria, max/min
- [x] Card Jardin (HN31): temp, humedad, bateria, max/min
- [x] Card Remoto (GW110): temp, humedad, presion, max/min
- [x] Status del servidor
- [x] Icono del clima en card Principal
- [x] Estructuras de datos para sensores remotos (RemoteSensorData, RemoteGatewayData)

### Pantallas de Detalle
- [ ] Estructura base de pantalla detalle
- [ ] Pantalla Detalle Principal (todos los sensores)
- [ ] Pantalla Detalle Local
- [ ] Pantalla Detalle Jardin
- [ ] Pantalla Detalle Remoto
- [ ] Navegacion tap card → detalle

### Conectar API
- [ ] Endpoint multi-estacion (/api/current?station=X)
- [ ] Datos HN31 (sensor remoto)
- [ ] Datos GW110 (gateway remoto)
- [ ] Estado del servidor

### Funcionalidad
- [ ] Long-press para acciones secundarias
- [ ] Slider de brillo funcional
- [ ] Cache offline de ultimo dato
- [ ] Reconexion WiFi automatica

## Fase 2 (Futuro)

### Pantallas Especializadas
- [ ] Pantalla de Pronostico (5-7 dias)
- [ ] Pantalla de Alertas (lista completa)
- [ ] Pantalla de Almanaque (sol, luna, planetas)
- [ ] Pantalla de Viento (rosa de vientos)
- [ ] Pantalla de Lluvia (acumulados detallados)
- [ ] Pantalla de Calidad del Aire
- [ ] Pantalla de Admin del Servidor
- [ ] Pantalla de Configuracion mejorada

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
*Actualizado: 2026-07-15*
