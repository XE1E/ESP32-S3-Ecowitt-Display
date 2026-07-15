# ESP32-S3-Ecowitt-Display - Estado de Desarrollo

## Fecha: 2026-07-14

## Hardware
- **Dispositivo**: Waveshare ESP32-S3-Touch-LCD-7B
- **Display**: 7" 1024x600 RGB Panel
- **Touch**: GT911 Capacitive Touch Controller
- **PSRAM**: 8MB OPI
- **Flash**: 16MB

## Estado Actual

### FUNCIONANDO:
- Display RGB inicializa correctamente
- Colores muestran correctos (R, G, B verificados)
- Backlight funciona via CH422G PWM
- LVGL renderiza la UI del wizard
- Caracteres sin acentos se muestran bien

### NO FUNCIONANDO:
- **Touch GT911 no responde** - Problema principal pendiente

## Configuracion de Pines (Verificados del codigo original Waveshare)

### Display RGB:
```
DE:    GPIO 5
VSYNC: GPIO 3
HSYNC: GPIO 46
PCLK:  GPIO 7

Red:   GPIO 14, 38, 18, 17, 10 (R0-R4)
Green: GPIO 39, 0, 45, 48, 47, 21 (G0-G5)
Blue:  GPIO 1, 2, 42, 41, 40 (B0-B4)
```

### I2C (compartido por touch y CH422G):
```
SDA: GPIO 19
SCL: GPIO 20
```

### Touch GT911:
```
INT: GPIO 4
RST: Via CH422G IO1 (no GPIO directo)
I2C Address: 0x5D (cuando INT esta LOW durante reset)
```

### CH422G IO Expander (I2C 0x24):
```
IO0: No usado
IO1: Touch RST
IO2: Backlight PWM
IO3: LCD RST
IO4: SD Card CS
IO5: USB/CAN select
```

## Timings del Display RGB (del codigo original):
```cpp
hsync_polarity: 0
hsync_front_porch: 48
hsync_pulse_width: 162
hsync_back_porch: 152
vsync_polarity: 0
vsync_front_porch: 3
vsync_pulse_width: 45
vsync_back_porch: 13
pclk_active_neg: 1
pixel_clock: 30MHz
```

## Lo que se ha probado para el Touch:

1. **Implementacion manual del GT911** - No funciono
   - Reset via GPIO directo (pines incorrectos)
   - Reset via CH422G IO1

2. **Secuencia de reset ANTERIOR (incorrecta)**:
   ```cpp
   pinMode(TOUCH_INT, OUTPUT);
   digitalWrite(TOUCH_INT, LOW);  // Select address 0x5D
   ch422g_set_pin(1, false);      // RST LOW  <-- ERROR: INT debe ponerse LOW despues de RST LOW
   delay(100);
   ch422g_set_pin(1, true);       // RST HIGH
   delay(200);
   pinMode(TOUCH_INT, INPUT);
   delay(50);
   ```

3. **Libreria GT911 Lite** - No funciono
   - No soporta registros de 16 bits del GT911
   - No permite configurar direccion I2C

4. **I2C Scanner** - No se pudo verificar (Serial no muestra output)

## Solucion Implementada (2026-07-14):

### Nueva secuencia de reset (correcta segun Waveshare):
```cpp
// 1. RST LOW primero
ch422g_set_pin(CH422G_TOUCH_RST_PIN, false);
delay(10);

// 2. INT LOW (selecciona direccion 0x5D)
pinMode(TOUCH_INT, OUTPUT);
digitalWrite(TOUCH_INT, LOW);
delay(10);

// 3. RST HIGH - controlador arranca
ch422g_set_pin(CH422G_TOUCH_RST_PIN, true);
delay(100);

// 4. Liberar INT (como INPUT)
pinMode(TOUCH_INT, INPUT);
delay(100);
```

### Driver GT911 reescrito:
- Comunicacion I2C directa con registros de 16 bits
- Verifica ambas direcciones I2C (0x5D y 0x14)
- Lee Product ID para confirmar comunicacion
- Debug detallado en Serial

## Problemas Conocidos:

1. **Serial USB CDC no muestra mensajes** despues del bootloader
   - Bootloader muestra: ESP-ROM, Build, rst, boot info
   - Mensajes de Serial.println() no aparecen
   - Puede ser problema de timing con USB CDC

2. **Touch no detectado**
   - El GT911 puede no estar respondiendo al I2C
   - La secuencia de reset puede ser incorrecta
   - Los pines I2C pueden necesitar verificacion fisica

## Proximos Pasos Sugeridos:

1. **Verificar I2C fisicamente**:
   - Usar un analizador logico o multimetro
   - Confirmar que SDA/SCL estan en GPIO 19/20

2. **Probar I2C con ejemplo simple**:
   - Crear sketch minimo solo con I2C scanner
   - Sin display, sin LVGL, solo scan I2C

3. **Verificar alimentacion del touch**:
   - El touch puede necesitar voltaje especifico
   - Verificar que CH422G esta inicializado antes del touch

4. **Probar otra libreria GT911**:
   - TAMC_GT911 (original, mas completa)
   - Verificar ejemplos especificos de Waveshare

5. **Revisar esquematico del hardware**:
   - Waveshare puede tener documentacion de pines
   - Verificar si hay pull-ups necesarios en I2C

## Archivos Clave:

- `config.h` - Pines del hardware
- `display.h` - Inicializacion display y CH422G
- `touch.h` - Driver GT911 (actualmente usa GT911_Lite)
- `ui_wizard.h` - UI del wizard de configuracion
- `unused_waveshare/` - Codigo original ESP-IDF de Waveshare

## Compilacion:

```bash
FQBN: esp32:esp32:esp32s3:CDCOnBoot=cdc,USBMode=hwcdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,FlashMode=qio
```

## Librerias Requeridas:
- lvgl 8.3.11
- GFX Library for Arduino 1.3.7
- ArduinoJson 6.x
- Adafruit BME280 Library
- Adafruit Unified Sensor
- GT911 Lite 1.0.2
