# Simbolos Nativos de LVGL

LVGL incluye simbolos integrados en las fuentes Montserrat. No requieren fuentes adicionales.

## Simbolos Disponibles

### Navegacion y UI

| Simbolo | Macro | Uso |
|---------|-------|-----|
| ⏴ | `LV_SYMBOL_LEFT` | Flecha izquierda |
| ⏵ | `LV_SYMBOL_RIGHT` | Flecha derecha |
| ⏶ | `LV_SYMBOL_UP` | Flecha arriba |
| ⏷ | `LV_SYMBOL_DOWN` | Flecha abajo |
| ✓ | `LV_SYMBOL_OK` | Confirmacion |
| ✕ | `LV_SYMBOL_CLOSE` | Cerrar |
| ⟳ | `LV_SYMBOL_REFRESH` | Actualizar |
| ⚙ | `LV_SYMBOL_SETTINGS` | Configuracion |
| 🏠 | `LV_SYMBOL_HOME` | Inicio |
| + | `LV_SYMBOL_PLUS` | Agregar |
| − | `LV_SYMBOL_MINUS` | Quitar |

### Estado y Conectividad

| Simbolo | Macro | Uso |
|---------|-------|-----|
| 📶 | `LV_SYMBOL_WIFI` | WiFi |
| 🔋 | `LV_SYMBOL_BATTERY_FULL` | Bateria llena |
| 🔋 | `LV_SYMBOL_BATTERY_3` | Bateria 75% |
| 🔋 | `LV_SYMBOL_BATTERY_2` | Bateria 50% |
| 🔋 | `LV_SYMBOL_BATTERY_1` | Bateria 25% |
| 🔋 | `LV_SYMBOL_BATTERY_EMPTY` | Bateria vacia |
| 🔌 | `LV_SYMBOL_CHARGE` | Cargando |
| ⚠ | `LV_SYMBOL_WARNING` | Advertencia |
| 🔔 | `LV_SYMBOL_BELL` | Notificacion |

### Multimedia

| Simbolo | Macro | Uso |
|---------|-------|-----|
| ▶ | `LV_SYMBOL_PLAY` | Reproducir |
| ⏸ | `LV_SYMBOL_PAUSE` | Pausar |
| ⏹ | `LV_SYMBOL_STOP` | Detener |
| ⏮ | `LV_SYMBOL_PREV` | Anterior |
| ⏭ | `LV_SYMBOL_NEXT` | Siguiente |
| 🔊 | `LV_SYMBOL_VOLUME_MAX` | Volumen alto |
| 🔉 | `LV_SYMBOL_VOLUME_MID` | Volumen medio |
| 🔇 | `LV_SYMBOL_MUTE` | Silencio |

### Edicion

| Simbolo | Macro | Uso |
|---------|-------|-----|
| ✏ | `LV_SYMBOL_EDIT` | Editar |
| 🗑 | `LV_SYMBOL_TRASH` | Eliminar |
| 💾 | `LV_SYMBOL_SAVE` | Guardar |
| 📁 | `LV_SYMBOL_DIRECTORY` | Carpeta |
| 📄 | `LV_SYMBOL_FILE` | Archivo |
| 📋 | `LV_SYMBOL_PASTE` | Pegar |
| ✂ | `LV_SYMBOL_CUT` | Cortar |
| 📑 | `LV_SYMBOL_COPY` | Copiar |

### Otros

| Simbolo | Macro | Uso |
|---------|-------|-----|
| 👁 | `LV_SYMBOL_EYE_OPEN` | Visible |
| 👁 | `LV_SYMBOL_EYE_CLOSE` | Oculto |
| 🔒 | `LV_SYMBOL_LOCK` | Bloqueado (NO disponible por defecto) |
| ⏱ | `LV_SYMBOL_TINT` | Gota (NO disponible por defecto) |
| 💡 | `LV_SYMBOL_POWER` | Energia |
| ☰ | `LV_SYMBOL_LIST` | Lista/Menu |
| ⊞ | `LV_SYMBOL_NEW_LINE` | Nueva linea |
| ⌫ | `LV_SYMBOL_BACKSPACE` | Borrar |
| ⏎ | `LV_SYMBOL_KEYBOARD` | Teclado |

## Uso en Codigo

```c
// Como texto en labels
lv_label_set_text(label, LV_SYMBOL_WIFI " Conectado");

// Concatenar con texto
lv_label_set_text(label, LV_SYMBOL_WARNING " Alerta!");

// En botones
lv_obj_t *btn = lv_btn_create(parent);
lv_obj_t *lbl = lv_label_create(btn);
lv_label_set_text(lbl, LV_SYMBOL_SETTINGS);

// Multiples simbolos
lv_label_set_text(label, LV_SYMBOL_OK " " LV_SYMBOL_WIFI);
```

## Uso en Este Proyecto

| Elemento | Simbolo | Ubicacion |
|----------|---------|-----------|
| WiFi status | `LV_SYMBOL_WIFI` | Header |
| Settings button | `LV_SYMBOL_SETTINGS` | Header |
| Temp max | `LV_SYMBOL_UP` | Panel temperatura |
| Temp min | `LV_SYMBOL_DOWN` | Panel temperatura |
| Alertas | `LV_SYMBOL_WARNING` | Panel alertas |
| OK status | `LV_SYMBOL_OK` | Panel alertas |
| Refresh | `LV_SYMBOL_REFRESH` | Header (opcional) |
| Theme toggle | `LV_SYMBOL_EYE_OPEN/CLOSE` | Header |
| Navegacion | `LV_SYMBOL_LEFT/RIGHT` | Footer (opcional) |

## Notas

- Los simbolos estan incluidos en las fuentes Montserrat de LVGL
- No todos los simbolos estan habilitados por defecto
- Para habilitar mas simbolos, editar `lv_conf.h`:
  ```c
  #define LV_FONT_MONTSERRAT_XX  1  // Tamano deseado
  ```
- Los simbolos escalan con el tamano de fuente
