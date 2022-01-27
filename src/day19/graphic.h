#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

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
// 不显示颜色
#define COLOR_NONE -1

// 一个字符的宽和高
#define FONT_WIDE 8
#define FONT_HIGH 16
// 图层的最大数量
#define SHEET_MAX_CNT 256
// 未使用图层的标志
#define SHEET_UNUSE 0
// 正在使用图层的标志
#define SHEET_USING 1
// 隐藏图层的标志
#define SHEET_HIDDEN -1
// 透明颜色的标志
#define SHEET_OPAQUE -1
#define SHEET_TOP SHEET_HEIGHT_MAX
#define SHEET_HEIGHT_MIN SHEET_OPAQUE
#define SHEET_HEIGHT_MAX 256
#define SHEET_IS_TOP 1
#define SHEET_NOT_TOP 0

#define CONTAINER_OTHER 0

#define CONTAINER_IS_WINDOW 0xf0000000

// 桌面背景色
#define DESKTOP_COLOR COLOR_LIGHT_DARK_BLUE

typedef struct SheetInfo {
    int wide, high, posx, posy, height
    //  宽    高    x坐标  y坐标  透明度
} SheetInfo;

typedef struct Sheet {
    char* buf;
    char tag;
    char isTop;
    SheetInfo info;
} Sheet;

typedef struct SheetManger {
    unsigned char *vram, *sheetMap;
    unsigned wide, high, cnt, area;
    Sheet* pSheet[SHEET_MAX_CNT];
    Sheet sheet[SHEET_MAX_CNT];
} SheetManger;
void sheetMangerInit(SheetManger* manger);

// 获得图层管理员
SheetManger* getSheetManger();
// 获得一个空的图层, 没有了返回NULL
Sheet* getEmptySheet();
// 初始化图层
void emptySheetInit(Sheet* sheet, unsigned char* buf, SheetInfo info);
// 释放图层
void sheetFree(Sheet* sheet);
// 改变图层权重
int sheetChangeHeight(Sheet* sheet, int height);
// 刷新某个图层
void sheetRefresh(Sheet* sheet, int x, int y, int wide, int high);
// 根据坐标刷新部分图层
void sheetRefreshSub(int x, int y, unsigned wide, unsigned high, int height);
// 移动图层
void sheetMove(Sheet* sheet, int x, int y);
// 初始化画板辅助函数, 也可自定义颜色
void setPalette(int start, int end, unsigned char* rgb);
// 获得最上层图层, 不包含鼠标的图层
Sheet* getTopSheet();
// 初始化画板
void initPalette(void);

// 绘制矩形
void drawRect(Sheet* sheet, const unsigned x, const unsigned int y,
              const unsigned wide, const unsigned high,
              const unsigned char COLOR);

// 初始化桌面信息
Sheet* initDesktop();

// 在x, y位置放置一个字符 ch
void putChar(const char ch, Sheet* sheet, const int x, const int y,
             const char color);

// 在x, y位置放置字符串
void putString(const char* str, Sheet* sheet, int x, int y, const char color);

#endif
