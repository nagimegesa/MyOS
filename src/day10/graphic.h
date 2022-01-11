#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

// 储存屏幕信息的结构体
struct Screen {
    int high, wide;
    char *startAddr;  // 在显卡中的起始位置
    char color;
};

// 初始化画板辅助函数, 也可自定义颜色
void setPalette(int start, int end, unsigned char *rgb);

// 初始化画板
void initPalette(void);

// 绘制矩形
void drawRect(struct Screen screen, const unsigned x, const unsigned int y,
              const unsigned wide, const unsigned high,
              const unsigned char COLOR);

// 绘制桌面
void drawDesktop(struct Screen screen);

// 初始化桌面信息
void intiDesktop(struct Screen *screen);

// 在x, y位置放置一个字符 ch
void putChar(const char ch, const struct Screen screen, const int x,
             const int y, const char color);

// 在x, y位置放置字符串
void putString(const char *str, const struct Screen screen, int x, int y,
               const char color);

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

#define FONT_WIDE 8
#define FONT_HIGH 16

#endif
