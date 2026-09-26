/*
 * 模拟器用的 LVGL 配置。未列出的选项取 LVGL 默认值（见 lv_conf_internal.h）。
 * 影响显示效果的配置（颜色深度、字体）必须和固件保持一致。
 */
#ifndef LV_CONF_H
#define LV_CONF_H

/* 与 ST7735 真机一致：RGB565 */
#define LV_COLOR_DEPTH 16

#define LV_USE_STDLIB_MALLOC LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_CLIB

#define LV_USE_OS LV_OS_NONE

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1

#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_UNSCII_8 1
#define LV_FONT_DEFAULT &lv_font_montserrat_12

#ifdef SIM_SDL
#define LV_USE_SDL 1
#endif

#endif /* LV_CONF_H */
