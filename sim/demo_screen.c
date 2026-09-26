#include "demo_screen.h"

static void add_color_block(lv_obj_t *parent, lv_color_t color, int32_t x)
{
    lv_obj_t *block = lv_obj_create(parent);
    lv_obj_remove_style_all(block);
    lv_obj_set_size(block, 20, 12);
    lv_obj_set_pos(block, x, 0);
    lv_obj_set_style_bg_color(block, color, 0);
    lv_obj_set_style_bg_opa(block, LV_OPA_COVER, 0);
}

void demo_screen_create(lv_obj_t *scr)
{
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x101820), 0);
    lv_obj_set_style_text_color(scr, lv_color_white(), 0);

    /* 1 像素白框：上板时检查四边是否都完整可见，用来确认显示偏移 */
    lv_obj_set_style_border_color(scr, lv_color_white(), 0);
    lv_obj_set_style_border_width(scr, 1, 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "c3-weather-clock");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);

    lv_obj_t *ver = lv_label_create(scr);
    lv_label_set_text_fmt(ver, "M0 sim  LVGL %d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR,
                          LVGL_VERSION_PATCH);
    lv_obj_align(ver, LV_ALIGN_TOP_MID, 0, 28);

    /* 色条：红 绿 蓝 白。真机上如果红蓝对调，说明需要切换 BGR 顺序 */
    lv_obj_t *bar = lv_obj_create(scr);
    lv_obj_remove_style_all(bar);
    lv_obj_set_size(bar, 80, 12);
    lv_obj_align(bar, LV_ALIGN_BOTTOM_MID, 0, -8);
    add_color_block(bar, lv_color_hex(0xFF0000), 0);
    add_color_block(bar, lv_color_hex(0x00FF00), 20);
    add_color_block(bar, lv_color_hex(0x0000FF), 40);
    add_color_block(bar, lv_color_hex(0xFFFFFF), 60);
}
