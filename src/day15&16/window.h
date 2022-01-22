#ifndef __WINDOW_H__
#define __WINDOW_H__

#define WINDOW_MIN_WIDE 50
#define WINDOW_MIN_HIGH 50
#define WINDOW_MAX_WIDE 300
#define WINDOW_MAX_HIGH 300
#define WINDOW_TITLE_HIGH 16
#define WINDOW_IS_TOP 1
#define WINDOW_NOT_TOP 0
struct Sheet;

typedef struct BaseWindow {
    struct Sheet* sheet;
} BaseWindow;
typedef struct WritableWindow {
    BaseWindow* window;
    unsigned chx, chy;
    char isTop;
} WritableWindow;
BaseWindow* makeBaseWindow(const char* title, int x, int y, unsigned wide,
                           unsigned high);
WritableWindow* makeWriteableWindow(const char* title, int x, int y,
                                     unsigned wide, unsigned high);
void WindowSheetChangeHeight(struct Sheet* sheet, int height);
#endif