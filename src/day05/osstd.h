#ifndef __OSSTD_H__
#define __OSSTD_H__

#include "BaseFunction.h"

// 初始化画板辅助函数, 也可自定义颜色
void setPalette(int start, int end, unsigned char *rgb);

// 初始化画板
void initPalette(void);

// 绘制矩形
void drawRect(unsigned char *addr, const int xSize, const unsigned x,
              const unsigned int y, const unsigned wide, const unsigned high,
              const unsigned char COLOR);

// 储存屏幕信息的结构体
struct Screen {
    unsigned high, wide;
    char *startAddr;  // 在显卡中的起始位置
};

struct Mouse {
    int x, y;
    struct Screen* screen;
    char model[16][16];
};

// 绘制屏幕
void drawScreen(struct Screen screen);

// 初始化屏幕信息
void initScreen(struct Screen *screen);

// 在x, y位置放置一个字符 ch
void putChar(const char ch, const struct Screen screen, const int x,
             const int y, const char color);

void putString(const char *str, const struct Screen screen, int x, int y,
               const char color);

// 初始化鼠标
void initMouse(struct Mouse *mouse, struct Screen *screen);

// 绘制鼠标
void drawMouse(const struct Mouse mouse);

struct SEGMENT_DESCRIPTOR {
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
};

struct GATE_DESCRIPTOR {
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
};

// 初始化gdt和idt
void init_gdtidt(void);
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base,
                  int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);

// BIOS 颜色
#define COLOR_BLACK 0             // 黑
#define COLOR_RED 1               // 红
#define COLOR_GREEN 2             // 绿
#define COLOR_YELLOW 3            // 黄
#define COLOR_BLUE 4              // 蓝
#define COLOR_PURPLE 5            // 紫
#define COLOR_LIGHT_BLUE 6        // 浅蓝
#define COLOR_WHITE 7             // 白
#define COLOR_GRAY 8              // 灰
#define COLOR_DARK_RED 9          // 暗红
#define COLOR_DARK_GREEN 10       // 暗绿
#define COLOR_DARK_YELLOW 11      // 暗黄
#define COLOR_DARK_BLUE 12        // 暗蓝
#define COLOR_DARK_PURPLE 13      // 暗紫
#define COLOR_LIGHT_DARK_BLUE 14  // 浅暗蓝
#define COLOR_ARGENT 15           // 银色

#endif
