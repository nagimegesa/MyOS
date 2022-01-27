#ifndef __WINDOW_H__
#define __WINDOW_H__

#define WINDOW_MIN_WIDE 50
#define WINDOW_MIN_HIGH 50
#define WINDOW_MAX_WIDE 300
#define WINDOW_MAX_HIGH 300
#define WINDOW_TITLE_HIGH 16
#define WINDOW_IS_TOP 1
#define WINDOW_NOT_TOP 0
#define WINDOW_MAX_CNT 100
#define WINDOW_TOP SHEET_TOP

#define WINDOW_TYPE_NONE 0
#define WINDOW_TYPE_BASE 1
#define WINDOW_TYPE_CMD 2

#define WINDOW_FONT_SIDE 1

#define KEY_BACKSPACE 8
#define KEY_ENTER 10
#define KEY_TAB 9
#define KEY_ESC 27

#include "buf.h"
struct Sheet;

typedef struct WindowInfo {
    int type;
    int height;
    void* addr;
} WindowInfo;

Declare_List(WindowInfo);

typedef struct WindowManger {
    int windowCnt;
    ListPointer(WindowInfo) infoList;
} WindowManger;

typedef struct BaseWindow {
    struct Sheet* sheet;
    int windowType;
} BaseWindow;

typedef struct ConsleWindow {
    BaseWindow BaseWindow;
    char cursorStat;
    unsigned curx, cury;
} ConsleWindow;



void makeBaseWindow(BaseWindow* window, const char* title, int x, int y,
                    unsigned wide, unsigned high);

void makeConsleWindow(ConsleWindow* window, const char* title, int x, int y,
                      unsigned wide, unsigned high);

WindowManger* getWindowManger();
void WindowMangerInit();
void drawCursor(ConsleWindow* window);
void windowChangeHeight(void* window, int height, int windowType);
void windowRefresh(void* window, int x, int y, int wide, int high);
struct Sheet* getWindowSheet(void* window);
int getWindowType(void* window);
void* getTopWindow();
void windowInput(ConsleWindow* window, char ch);
void windowClean(void* window);
#endif
