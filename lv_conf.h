/**
 * lv_conf.h - LVGL Configuration
 *
 * For Waveshare ESP32-S3-Touch-LCD-7B (1024x600)
 * LVGL 8.3.x
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/* Color depth: 16 (RGB565) */
#define LV_COLOR_DEPTH 16

/* Swap the 2 bytes of RGB565 color */
#define LV_COLOR_16_SWAP 0

/* Enable more complex drawing routines */
#define LV_DRAW_COMPLEX 1

/* Enable anti-aliasing */
#define LV_ANTIALIAS 1

/*====================
   MEMORY SETTINGS
 *====================*/

/* Size of internal graphics buffer (in bytes)
 * Using PSRAM, can be larger */
#define LV_MEM_CUSTOM 1
#define LV_MEM_CUSTOM_INCLUDE <stdlib.h>
#define LV_MEM_CUSTOM_ALLOC malloc
#define LV_MEM_CUSTOM_FREE free
#define LV_MEM_CUSTOM_REALLOC realloc

/* Use custom memory pool size */
#define LV_MEM_SIZE (128U * 1024U)

/* Memory buffer for LVGL (disabled - using custom) */
#define LV_MEM_ADR 0
#define LV_MEM_POOL_INCLUDE <lvgl.h>
#define LV_MEM_POOL_ALLOC lv_mem_alloc

/*====================
   HAL SETTINGS
 *====================*/

/* Default display refresh period (ms) */
#define LV_DISP_DEF_REFR_PERIOD 16

/* Input device read period (ms) */
#define LV_INDEV_DEF_READ_PERIOD 30

/* Use custom tick source - set to 0 for Waveshare lvgl_port */
#define LV_TICK_CUSTOM 0
#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE <Arduino.h>
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/*====================
   FEATURE CONFIG
 *====================*/

/* Drawing */
#define LV_USE_DRAW_MASKS 1

/* GPU */
#define LV_USE_GPU_STM32_DMA2D 0
#define LV_USE_GPU_NXP_PXP 0
#define LV_USE_GPU_NXP_VG_LITE 0
#define LV_USE_GPU_SDL 0

/*====================
   LOGGING
 *====================*/

/* Enable logging */
#define LV_USE_LOG 1
#if LV_USE_LOG
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #define LV_LOG_PRINTF 1
    #define LV_LOG_TRACE_MEM 0
    #define LV_LOG_TRACE_TIMER 0
    #define LV_LOG_TRACE_INDEV 0
    #define LV_LOG_TRACE_DISP_REFR 0
    #define LV_LOG_TRACE_EVENT 0
    #define LV_LOG_TRACE_OBJ_CREATE 0
    #define LV_LOG_TRACE_LAYOUT 0
    #define LV_LOG_TRACE_ANIM 0
#endif

/*====================
   ASSERTS
 *====================*/

#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_ASSERT_STYLE 0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ 0

/*====================
   DEBUG / MONITOR
 *====================*/

/* Show CPU usage and FPS */
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

/* Draw random colored rectangles over invalidated areas */
#define LV_USE_REFR_DEBUG 0

/*====================
   COMPILER SETTINGS
 *====================*/

#define LV_BIG_ENDIAN_SYSTEM 0
#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 4
#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY
#define LV_ATTRIBUTE_FAST_MEM
#define LV_ATTRIBUTE_DMA
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning
#define LV_USE_LARGE_COORD 0

/*====================
   FONT USAGE
 *====================*/

/* Montserrat fonts (built-in) */
#define LV_FONT_MONTSERRAT_8 0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 1
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 1

/* Montserrat supplemental */
#define LV_FONT_MONTSERRAT_12_SUBPX 0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0

/* Dejavu fonts */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0

/* Simsun CJK */
#define LV_FONT_SIMSUN_16_CJK 0

/* Unscii fonts */
#define LV_FONT_UNSCII_8 0
#define LV_FONT_UNSCII_16 0

/* Custom fonts */
#define LV_FONT_CUSTOM_DECLARE

/* Default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_16

/* Enable font subpixel rendering */
#define LV_FONT_SUBPX_BGR 0

/* Enable font anti-aliasing */
#define LV_FONT_FMT_TXT_LARGE 0
#define LV_USE_FONT_COMPRESSED 0
#define LV_USE_FONT_PLACEHOLDER 1

/*====================
   TEXT SETTINGS
 *====================*/

/* String character encoding */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/* Characters for line breaking */
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/* Text line length estimation */
#define LV_TXT_LINE_BREAK_LONG_LEN 0
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/* Arabic/Persian text support */
#define LV_TXT_COLOR_CMD "#"

/* Bidirectional text support */
#define LV_USE_BIDI 0
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*====================
   WIDGET USAGE
 *====================*/

/* Base widgets */
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CANVAS 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_IMG 1
#define LV_USE_LABEL 1
#define LV_USE_LINE 1
#define LV_USE_ROLLER 1
#define LV_USE_SLIDER 1
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TABLE 1

/* Extra widgets */
#define LV_USE_ANIMIMG 0
#define LV_USE_CALENDAR 1
#define LV_USE_CALENDAR_HEADER_ARROW 1
#define LV_USE_CALENDAR_HEADER_DROPDOWN 1
#define LV_USE_CHART 1
#define LV_USE_COLORWHEEL 0
#define LV_USE_IMGBTN 1
#define LV_USE_KEYBOARD 1
#define LV_USE_LED 1
#define LV_USE_LIST 1
#define LV_USE_MENU 1
#define LV_USE_METER 1
#define LV_USE_MSGBOX 1
#define LV_USE_SPAN 1
#define LV_USE_SPINBOX 1
#define LV_USE_SPINNER 1
#define LV_USE_TABVIEW 1
#define LV_USE_TILEVIEW 0
#define LV_USE_WIN 0

/*====================
   THEMES
 *====================*/

/* Default theme */
#define LV_USE_THEME_DEFAULT 1
#define LV_THEME_DEFAULT_DARK 0
#define LV_THEME_DEFAULT_GROW 1
#define LV_THEME_DEFAULT_TRANSITION_TIME 80

/* Basic theme (simpler, smaller) */
#define LV_USE_THEME_BASIC 1

/* Mono theme */
#define LV_USE_THEME_MONO 0

/*====================
   LAYOUTS
 *====================*/

#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*====================
   3RD PARTY LIBS
 *====================*/

/* File system interface */
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_FATFS 0

/* PNG decoder */
#define LV_USE_PNG 0

/* BMP decoder */
#define LV_USE_BMP 0

/* JPG + split JPG decoder */
#define LV_USE_SJPG 0

/* GIF decoder */
#define LV_USE_GIF 0

/* QR code */
#define LV_USE_QRCODE 0

/* FreeType library */
#define LV_USE_FREETYPE 0

/* Rlottie library */
#define LV_USE_RLOTTIE 0

/* FFmpeg */
#define LV_USE_FFMPEG 0

#endif /* LV_CONF_H */
