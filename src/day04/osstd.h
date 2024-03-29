
#ifndef __OSSTD__H
#define __OSSTD__H

#include "BaseFunction.h"

// 初始化画板辅助函数, 也可自定义颜色
void setPalette(int start, int end, unsigned char *rgb);

// 初始化画板
void initPalette(void);

// 绘制矩形
void drawRect(unsigned char *addr, int xSize, int x, int y, unsigned wide,
              unsigned high, unsigned char COLOR);

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
