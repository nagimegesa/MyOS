#include "osstd.h"

// 初始化调色板
void initPalette(void) {
    static unsigned char table_rgb[16 * 3] = {
        0x00, 0x00, 0x00, /*  0:黑 */
        0xff, 0x00, 0x00, /*  1:亮红 */
        0x00, 0xff, 0x00, /*  2:亮绿 */
        0xff, 0xff, 0x00, /*  3:亮黄 */
        0x00, 0x00, 0xff, /*  4:亮蓝 */
        0xff, 0x00, 0xff, /*  5:亮紫 */
        0x00, 0xff, 0xff, /*  6:浅亮蓝 */
        0xff, 0xff, 0xff, /*  7:白 */
        0xc6, 0xc6, 0xc6, /*  8:亮灰 */
        0x84, 0x00, 0x00, /*  9:暗红 */
        0x00, 0x84, 0x00, /* 10:暗绿 */
        0x84, 0x84, 0x00, /* 11:暗黄 */
        0x00, 0x00, 0x84, /* 12:暗青 */
        0x84, 0x00, 0x84, /* 13:暗紫 */
        0x00, 0x84, 0x84, /* 14:浅暗蓝 */
        0x84, 0x84, 0x84  /* 15:暗灰 */
    };
    setPalette(0, 15, table_rgb);
    return;
}

// 设置调色板颜色
void setPalette(int start, int end, unsigned char *rgb) {
    int eflags = getLockTag(); /* 记录中断许可标志的值*/
    setLock();                 /* 将中断许可标志置为0，禁止中断 */
    writePort8(0x03c8, start);
    for (int i = start; i <= end; i++) {
        writePort8(0x03c9, rgb[0] / 4);
        writePort8(0x03c9, rgb[1] / 4);
        writePort8(0x03c9, rgb[2] / 4);
        rgb += 3;
    }
    setLockTag(eflags); /* 复原中断许可标志 */
    return;
}

void drawRect(unsigned char *addr, int xSize, int x, int y, unsigned high,
              unsigned wide, unsigned char COLOR) {
    const int x1 = x + wide, y1 = y + high;
    for (int i = y; i <= y1; ++i) {
        for (int j = x; j <= x1; ++j) {
            addr[i * xSize + j] = COLOR;
        }
    }
    return;
}