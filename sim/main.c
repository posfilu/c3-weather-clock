/*
 * LVGL PC 模拟器入口。
 *   截图模式：c3wc_sim --out shot.png [--scale 4]
 *   SDL 模式（编译时 -DSIM_SDL=ON）：c3wc_sim，方向键 / 回车模拟摇杆，关闭窗口退出
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "demo_screen.h"
#include "lvgl.h"
#include "png_writer.h"

#define DISP_W 160
#define DISP_H 80

static uint32_t now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000u + ts.tv_nsec / 1000000u);
}

#ifdef SIM_SDL

int main(void)
{
    lv_init();
    lv_tick_set_cb(now_ms);

    lv_display_t *disp = lv_sdl_window_create(DISP_W, DISP_H);
    if (!disp) {
        fprintf(stderr, "failed to create SDL window (DISPLAY / WAYLAND_DISPLAY 是否可用？)\n");
        return 1;
    }
    lv_sdl_window_set_zoom(disp, 4);
    lv_sdl_window_set_title(disp, "c3-weather-clock sim");
    lv_sdl_keyboard_create();

    demo_screen_create(lv_screen_active());

    for (;;) {
        uint32_t idle_ms = lv_timer_handler();
        usleep((idle_ms ? idle_ms : 1) * 1000);
    }
}

#else /* 截图模式 */

static uint16_t framebuffer[DISP_W * DISP_H];
static uint8_t draw_buf[DISP_W * DISP_H * 2];

static void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    const int32_t w = lv_area_get_width(area);
    const uint32_t stride = lv_draw_buf_width_to_stride(w, LV_COLOR_FORMAT_RGB565);
    for (int32_t y = area->y1; y <= area->y2; y++) {
        memcpy(&framebuffer[y * DISP_W + area->x1], px_map + (size_t)(y - area->y1) * stride,
               (size_t)w * 2);
    }
    lv_display_flush_ready(disp);
}

static void usage(const char *argv0)
{
    fprintf(stderr, "usage: %s --out <file.png> [--scale N]\n", argv0);
}

int main(int argc, char **argv)
{
    const char *out = NULL;
    int scale = 4;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--out") == 0 && i + 1 < argc) {
            out = argv[++i];
        } else if (strcmp(argv[i], "--scale") == 0 && i + 1 < argc) {
            scale = atoi(argv[++i]);
        } else {
            usage(argv[0]);
            return 2;
        }
    }
    if (!out || scale < 1 || scale > 16) {
        usage(argv[0]);
        return 2;
    }

    lv_init();
    lv_tick_set_cb(now_ms);

    lv_display_t *disp = lv_display_create(DISP_W, DISP_H);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, flush_cb);

    demo_screen_create(lv_screen_active());
    lv_refr_now(disp);

    const int margin = 2 * scale;
    if (png_write_rgb565(out, framebuffer, DISP_W, DISP_H, scale, margin) != 0) {
        fprintf(stderr, "failed to write %s\n", out);
        return 1;
    }
    printf("screenshot: %s (%dx%d display, scale %d, %dpx grey margin)\n", out, DISP_W * scale,
           DISP_H * scale, scale, margin);
    return 0;
}

#endif /* SIM_SDL */
