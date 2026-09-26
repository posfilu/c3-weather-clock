/*
 * 最小 PNG 编码器：zlib 流只用 stored（不压缩）块，省掉对 zlib / libpng 的依赖。
 * 160x80 放大 4 倍约 600KB，对截图来说足够。
 */
#include "png_writer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t crc_table[256];

static void crc_init(void)
{
    for (uint32_t n = 0; n < 256; n++) {
        uint32_t c = n;
        for (int k = 0; k < 8; k++) {
            c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
        }
        crc_table[n] = c;
    }
}

static uint32_t crc_update(uint32_t crc, const uint8_t *buf, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        crc = crc_table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}

static void put_be32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static int write_chunk(FILE *f, const char type[4], const uint8_t *data, uint32_t len)
{
    uint8_t hdr[8];
    put_be32(hdr, len);
    memcpy(hdr + 4, type, 4);
    uint32_t crc = crc_update(0xFFFFFFFFu, (const uint8_t *)type, 4);
    crc = crc_update(crc, data, len) ^ 0xFFFFFFFFu;
    uint8_t tail[4];
    put_be32(tail, crc);
    if (fwrite(hdr, 1, 8, f) != 8) {
        return -1;
    }
    if (len && fwrite(data, 1, len, f) != len) {
        return -1;
    }
    return fwrite(tail, 1, 4, f) == 4 ? 0 : -1;
}

int png_write_rgb565(const char *path, const uint16_t *pixels, int width, int height, int scale)
{
    crc_init();

    const uint32_t out_w = (uint32_t)(width * scale);
    const uint32_t out_h = (uint32_t)(height * scale);
    const size_t row_len = 1 + (size_t)out_w * 3; /* 每行前面一个 filter 字节（0 = None） */
    const size_t raw_len = row_len * out_h;

    uint8_t *raw = malloc(raw_len);
    if (!raw) {
        return -1;
    }
    for (uint32_t y = 0; y < out_h; y++) {
        uint8_t *row = raw + y * row_len;
        row[0] = 0;
        for (uint32_t x = 0; x < out_w; x++) {
            uint16_t c = pixels[(y / scale) * width + (x / scale)];
            uint8_t r5 = (c >> 11) & 0x1F, g6 = (c >> 5) & 0x3F, b5 = c & 0x1F;
            row[1 + x * 3 + 0] = (uint8_t)((r5 << 3) | (r5 >> 2));
            row[1 + x * 3 + 1] = (uint8_t)((g6 << 2) | (g6 >> 4));
            row[1 + x * 3 + 2] = (uint8_t)((b5 << 3) | (b5 >> 2));
        }
    }

    /* zlib 流：2 字节头 + 若干 stored 块（每块最多 65535 字节）+ Adler-32 */
    const size_t nblocks = (raw_len + 65534) / 65535;
    const size_t z_len = 2 + nblocks * 5 + raw_len + 4;
    uint8_t *z = malloc(z_len);
    if (!z) {
        free(raw);
        return -1;
    }
    size_t zp = 0;
    z[zp++] = 0x78;
    z[zp++] = 0x01;
    uint32_t a = 1, b = 0;
    for (size_t off = 0; off < raw_len; off += 65535) {
        size_t n = raw_len - off < 65535 ? raw_len - off : 65535;
        z[zp++] = (off + n == raw_len) ? 1 : 0; /* BFINAL + BTYPE=00 */
        z[zp++] = (uint8_t)(n & 0xFF);
        z[zp++] = (uint8_t)(n >> 8);
        z[zp++] = (uint8_t)(~n & 0xFF);
        z[zp++] = (uint8_t)((~n >> 8) & 0xFF);
        memcpy(z + zp, raw + off, n);
        zp += n;
        for (size_t i = 0; i < n; i++) {
            a = (a + raw[off + i]) % 65521;
            b = (b + a) % 65521;
        }
    }
    put_be32(z + zp, (b << 16) | a);
    zp += 4;
    free(raw);

    uint8_t ihdr[13];
    put_be32(ihdr, out_w);
    put_be32(ihdr + 4, out_h);
    ihdr[8] = 8;  /* 位深 */
    ihdr[9] = 2;  /* 颜色类型：RGB */
    ihdr[10] = 0; /* 压缩方法 */
    ihdr[11] = 0; /* 过滤方法 */
    ihdr[12] = 0; /* 不隔行 */

    static const uint8_t signature[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    FILE *f = fopen(path, "wb");
    int rc = -1;
    if (f && fwrite(signature, 1, 8, f) == 8 && write_chunk(f, "IHDR", ihdr, 13) == 0 &&
        write_chunk(f, "IDAT", z, (uint32_t)zp) == 0 && write_chunk(f, "IEND", NULL, 0) == 0) {
        rc = 0;
    }
    if (f && fclose(f) != 0) {
        rc = -1;
    }
    free(z);
    return rc;
}
