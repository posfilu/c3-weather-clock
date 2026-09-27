#ifndef PNG_WRITER_H
#define PNG_WRITER_H

#include <stdint.h>

/*
 * 把 RGB565 帧缓冲写成 PNG（RGB8，不压缩），每个像素放大 scale 倍，
 * 四周再加 margin 像素宽的灰色衬边（在显示区域之外），
 * 这样在白底看图软件里也能看清屏幕最边缘的内容。
 * 返回 0 表示成功，-1 表示打开或写文件失败。
 */
int png_write_rgb565(const char *path, const uint16_t *pixels, int width, int height, int scale,
                     int margin);

#endif /* PNG_WRITER_H */
