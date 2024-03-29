#include "osstd.h"

#include "MyFont.h"

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

void drawRect(unsigned char *addr, const int xSize, const unsigned x,
              const unsigned y, const unsigned wide, const unsigned high,
              const unsigned char COLOR) {
    const int x1 = x + wide, y1 = y + high;
    for (int i = y; i <= y1; ++i) {
        for (int j = x; j <= x1; ++j) {
            addr[i * xSize + j] = COLOR;
        }
    }
    return;
}

void initScreen(struct Screen *screen) {
    screen->wide = *(short *)0x0ff4;
    screen->high = *(short *)0x0ff6;
    screen->startAddr = (char *)(*(int *)0x0ff8);
}

void drawScreen(struct Screen screen) {
    drawRect(screen.startAddr, screen.wide, 0, 0, screen.wide, screen.high,
             COLOR_LIGHT_DARK_BLUE);
}

// 在x, y位置放置一个字符 ch
void putChar(const char ch, const struct Screen screen, const int x,
             const int y, const char color) {
    char *addr = (char *)screen.startAddr;
    const char *model = __FontModel + 16 * ch;
    const unsigned xsize = screen.wide;
    for (int i = 0; i < 16; i++) {
        const char d = model[i];
        char *p = addr + (y + i) * xsize + x;
        if ((d & 0x80) != 0) p[0] = color;
        if ((d & 0x40) != 0) p[1] = color;
        if ((d & 0x20) != 0) p[2] = color;
        if ((d & 0x10) != 0) p[3] = color;
        if ((d & 0x08) != 0) p[4] = color;
        if ((d & 0x04) != 0) p[5] = color;
        if ((d & 0x02) != 0) p[6] = color;
        if ((d & 0x01) != 0) p[7] = color;
    }
    return;
}

void putString(const char *str, const struct Screen screen, int x, int y,
               const char color) {
    for (; *str != '\0'; ++str) {
        putChar(*str, screen, x, y, color);
        x += 8;
    }
}

// 初始化鼠标
void initMouse(struct Mouse *mouse, struct Screen *screen) {
    static char cursor[16][16] = {
        "**************..", "*OOOOOOOOOOO*...", "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....", "*OOOOOOOO*......", "*OOOOOOO*.......",
        "*OOOOOOO*.......", "*OOOOOOOO*......", "*OOOO**OOO*.....",
        "*OOO*..*OOO*....", "*OO*....*OOO*...", "*O*......*OOO*..",
        "**........*OOO*.", "*..........*OOO*", "............*OO*",
        ".............***"};
    mouse->screen = screen;
    mouse->x = screen->wide / 2;
    mouse->y = screen->high / 2;
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            if (cursor[i][j] == '*') {
                mouse->model[i][j] = COLOR_BLACK;
            } else if (cursor[i][j] == 'O') {
                mouse->model[i][j] = COLOR_WHITE;
            } else {
                mouse->model[i][j] = -1;
            }
        }
    }
    return;
}

// 绘制鼠标
void drawMouse(const struct Mouse mouse) {
    const int x = mouse.x, y = mouse.y;
    char *addr = mouse.screen->startAddr;
    const unsigned wide = mouse.screen->wide;
    for (int i = 0; i < 16; ++i) {
        const unsigned tmp = (y + i) * wide + x;
        for (int j = 0; j < 16; ++j) {
            if (mouse.model[i][j] != (char)-1) {
                addr[tmp + j] = mouse.model[i][j];
            }
        }
    }
}

void init_gdtidt(void) {
    struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)0x00270000;
    struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)0x0026f800;
    int i;

    /* GDT初始化 */
    for (i = 0; i < 8192; i++) {
        set_segmdesc(gdt + i, 0, 0, 0);
    }
    set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
    set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
    load_gdtr(0xffff, 0x00270000);

    /* IDT初始化 */
    for (i = 0; i < 256; i++) {
        set_gatedesc(idt + i, 0, 0, 0);
    }
    load_idtr(0x7ff, 0x0026f800);

    return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base,
                  int ar) {
    if (limit > 0xfffff) {
        ar |= 0x8000; /* G_bit = 1 */
        limit /= 0x1000;
    }
    sd->limit_low = limit & 0xffff;
    sd->base_low = base & 0xffff;
    sd->base_mid = (base >> 16) & 0xff;
    sd->access_right = ar & 0xff;
    sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->base_high = (base >> 24) & 0xff;
    return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector,
                  int ar) {
    gd->offset_low = offset & 0xffff;
    gd->selector = selector;
    gd->dw_count = (ar >> 8) & 0xff;
    gd->access_right = ar & 0xff;
    gd->offset_high = (offset >> 16) & 0xffff;
    return;
}