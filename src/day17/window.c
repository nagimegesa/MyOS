#include "window.h"

#include "graphic.h"
//#include "memory.h"
#include "timer.h"

typedef WindowManger Manger;
static void addWindow(void* addr, int type, int height);
static int cmp(WindowInfo info, WindowInfo data);
static void makeBaseWindowImpl(BaseWindow* window, const char* title, int x,
                               int y, unsigned wide, unsigned high, int tag);

Manger* getWindowManger() {
    static Manger manger;
    return &manger;
}

void WindowMangerInit() {
    Manger* manger = getWindowManger();
    manger->infoList = GetEmptyList(WindowInfo)(cmp);
    manger->windowCnt = 0;
}

void makeBaseWindow(BaseWindow* window, const char* title, int x, int y,
                    unsigned wide, unsigned high) {
    makeBaseWindowImpl(window, title, x, y, wide, high, 1);
}

void makeConsleWindow(ConsleWindow* window, const char* title, int x, int y,
                      unsigned wide, unsigned high) {
    makeBaseWindowImpl(&window->BaseWindow, title, x, y, wide, high, 0);
    window->cursorStat = 0;
    window->curx = 1;
    window->cury = WINDOW_TITLE_HIGH;
    window->BaseWindow.windowType = WINDOW_TYPE_CMD;
    addWindow(window, WINDOW_TYPE_CMD, window->BaseWindow.sheet->info.height);
}

void drawCursor(ConsleWindow* window) {
    Sheet* sheet = window->BaseWindow.sheet;
    if (window->cursorStat)
        drawRect(sheet, window->curx, window->cury, FONT_WIDE, FONT_HIGH,
                 COLOR_BLACK);
    else
        drawRect(sheet, window->curx, window->cury, FONT_WIDE, FONT_HIGH,
                 COLOR_GRAY);
    sheetRefresh(sheet, window->curx, window->cury, FONT_WIDE, FONT_HIGH);
    window->cursorStat = !window->cursorStat;
}

void windowChangeHeight(void* window, int height, int windowType) {
    Sheet* sheet = getWindowSheet(window);
    Manger* manger = getWindowManger();
    int oldHeight = sheet->info.height;
    height = sheetChangeHeight(sheet, height);
    WindowInfo info;
    info.height = oldHeight;
    info.addr = window;
    ListErase(WindowInfo)(manger->infoList, info);
    info.height = height;
    info.type = windowType;
    ListInsert(WindowInfo)(manger->infoList, info);
}

static void addWindow(void* window, int type, int height) {
    Manger* manger = getWindowManger();
    WindowInfo info;
    info.addr = window;
    info.height = height;
    info.type = type;
    ListInsert(WindowInfo)(manger->infoList, info);
    ++manger->windowCnt;
}

void* getTopWindow() {
    Manger* manger = getWindowManger();
    return ListGetFirst(WindowInfo)(manger->infoList)->data.addr;
}

void windowRefresh(void* window, int x, int y, int wide, int high) {
    Sheet* sheet = getWindowSheet(window);
    sheetRefresh(sheet, x, y, wide, high);
}

Sheet* getWindowSheet(void* window) { return ((BaseWindow*)window)->sheet; }
int getWindowType(void* window) { return ((BaseWindow*)window)->windowType; }

void windowInput(ConsleWindow* window, char ch) {
    static int isEnter = 0, lastx = 0, lasty = WINDOW_TITLE_HIGH;
    int chx = window->curx, chy = window->cury;
    int x = chx, y = chy;
    Sheet* sheet = getWindowSheet(window);
    int wide = sheet->info.wide;
    if (ch == KEY_ENTER || ch == KEY_TAB) {
        return;
    } else if (ch == KEY_BACKSPACE) {
        x = y == WINDOW_TITLE_HIGH
                ? WINDOW_FONT_SIDE
                : wide - (wide - WINDOW_FONT_SIDE * 2) % FONT_WIDE - FONT_WIDE;
        y = y == WINDOW_TITLE_HIGH ? y : y - FONT_HIGH;
    } else {
        x += FONT_WIDE;
        if (x >= wide - 2 * WINDOW_FONT_SIDE - FONT_WIDE) {
            x = WINDOW_FONT_SIDE;
            y += FONT_HIGH;
        };
    }
    window->curx = x;
    window->cury = y;
    drawRect(sheet, chx, chy, FONT_WIDE, FONT_HIGH, COLOR_GRAY);
    if (ch != KEY_BACKSPACE) putChar(ch, sheet, chx, chy, COLOR_BLACK);
    windowRefresh(window, chx, chy, FONT_WIDE, FONT_HIGH);
}

Define_List(WindowInfo);

static int cmp(WindowInfo info, WindowInfo data) {
    return info.height > data.height && info.addr != data.addr;
}
static void makeBaseWindowImpl(BaseWindow* window, const char* title, int x,
                               int y, unsigned wide, unsigned high, int tag) {
    static char closebtn[14][16] = {
        "OOOOOOOOOOOOOOO@", "OQQQQQQQQQQQQQ$@", "OQQQQQQQQQQQQQ$@",
        "OQQQ@@QQQQ@@QQ$@", "OQQQQ@@QQ@@QQQ$@", "OQQQQQ@@@@QQQQ$@",
        "OQQQQQQ@@QQQQQ$@", "OQQQQQ@@@@QQQQ$@", "OQQQQ@@QQ@@QQQ$@",
        "OQQQ@@QQQQ@@QQ$@", "OQQQQQQQQQQQQQ$@", "OQQQQQQQQQQQQQ$@",
        "O$$$$$$$$$$$$$$@", "@@@@@@@@@@@@@@@@"};
    window->sheet = getEmptySheet();
    SheetInfo info;
    info.high = high < WINDOW_MIN_HIGH ? WINDOW_MIN_HIGH : high;
    info.wide = wide < WINDOW_MIN_WIDE ? WINDOW_MIN_WIDE : wide;
    info.height = -1;
    info.posx = x;
    info.posy = y;
    char* buf = memoryAlloc(wide * high);
    emptySheetInit(window->sheet, buf, info);
    drawRect(window->sheet, 0, 0, wide, high, COLOR_GRAY);
    drawRect(window->sheet, 0, 0, wide, 14, COLOR_BLACK);
    putString(title, window->sheet, 0, 0, COLOR_WHITE);
    for (int i = 0; i < 14; ++i) {
        const unsigned tmp = i * wide + wide - 16;
        for (int j = 0; j < 16; ++j) {
            if (closebtn[i][j] == 'O')
                buf[tmp + j] = COLOR_WHITE;
            else if (closebtn[i][j] == '@')
                buf[tmp + j] = COLOR_BLACK;
            else if (closebtn[i][j] == 'Q')
                buf[tmp + j] = COLOR_GRAY;
            else
                buf[tmp + j] = COLOR_ARGENT;
        }
    }
    if (tag) {
        window->windowType = WINDOW_TYPE_BASE;
        addWindow(window, WINDOW_TYPE_BASE, window->sheet->info.height);
    }
}