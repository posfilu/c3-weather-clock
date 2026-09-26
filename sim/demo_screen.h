#ifndef DEMO_SCREEN_H
#define DEMO_SCREEN_H

#include "lvgl.h"

/*
 * M0 占位界面：项目名、LVGL 版本，外加一条 RGB 色条，
 * M1 上板时对照这张截图检查颜色顺序（RGB/BGR）和显示偏移。
 * M2 起由开发者的 UI 组件取代。
 */
void demo_screen_create(lv_obj_t *scr);

#endif /* DEMO_SCREEN_H */
