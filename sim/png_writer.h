#ifndef PNG_WRITER_H
#define PNG_WRITER_H

#include <stdint.h>

/*
 * 把 RGB565 帧缓冲写成 PNG（RGB8，不压缩），每个像素放大 scale 倍。
 * 返回 0 表示成功，-1 表示打开或写文件失败。
 */
int png_write_rgb565(const char *path, const uint16_t *pixels, int width, int height, int scale);

#endif /* PNG_WRITER_H */
