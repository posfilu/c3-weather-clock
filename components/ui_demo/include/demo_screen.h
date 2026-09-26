#ifndef DEMO_SCREEN_H
#define DEMO_SCREEN_H

#include "lvgl.h"

/*
 * 演示界面：项目名、LVGL 版本、1 像素白边框和一条 RGB 色条。
 * 固件和模拟器共用这份代码，M1 上板时对照模拟器截图检查显示偏移、颜色顺序和字节序。
 * M2 起由开发者的 UI 组件取代。
 */
void demo_screen_create(lv_obj_t *scr);

#endif /* DEMO_SCREEN_H */
