#include "window.h"

#include <stdio.h>
#include <string.h>

#include "file.h"
#include "graphic.h"
#include "timer.h"

typedef WindowManger Manger;
static void addWindow(void* addr, int type, int height);
static int cmp(WindowInfo info, WindowInfo data);
static void makeBaseWindowImpl(BaseWindow* window, const char* title, int x,
                               int y, unsigned wide, unsigned high, int tag);
static void scrollWindow(ConsleWindow* window);
static void dispatchCmd(ConsleWindow* window, const char* cmd);
static void windowPutChar(ConsleWindow* window, char ch);
static void windowPutString(ConsleWindow* window, const char* str);
static void showFile(ConsleWindow* window);
static showFileConent(ConsleWindow* window, const char* str);

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
    windowPutChar(window, '>');
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
    static char buf[50], top = 0;
    if (ch == KEY_ENTER) {
        dispatchCmd(window, buf);
        top = 0;
        memset(buf, 0, sizeof(buf));
    } else if (ch != KEY_BACKSPACE)
        buf[top++] = ch;
    else if (ch == KEY_BACKSPACE) {
        buf[top] = '\0';
        top = top == 0 ? 0 : top - 1;
    }
    windowPutChar(window, ch);
    if (ch == KEY_ENTER) windowPutChar(window, '>');
}
void windowClean(void* window) {
    Sheet* sheet = getWindowSheet(window);
    drawRect(sheet, 0, WINDOW_TITLE_HIGH, sheet->info.wide,
             sheet->info.high - WINDOW_TITLE_HIGH - 1, COLOR_GRAY);
    if (getWindowType(window) == WINDOW_TYPE_CMD) {
        ConsleWindow* w = (ConsleWindow*)window;
        w->curx = 0;
        w->cury = WINDOW_TITLE_HIGH;
    }
    windowRefresh(window, 0, 0, sheet->info.wide, sheet->info.high);
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
    drawRect(window->sheet, 0, 0, wide, WINDOW_TITLE_HIGH - 1, COLOR_BLACK);
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
static void dispatchCmd(ConsleWindow* window, const char* cmd) {
    int len = strlen(cmd);
    if (strlen(cmd) == 0) return;
    char buf[40];
    memset(buf, 0, sizeof(buf));
    if (strcmp(cmd, "mem") == 0) {
        unsigned size = getFreeMemorySize();
        sprintf(buf, "\nmemory size %d\n", size);
    } else if (strcmp(cmd, "ll") == 0) {
        showFile(window);
    } else if (strcmp(cmd, "cls") == 0) {
        windowClean(window);
    } else {
        int index = 0;
        char str[5], file[10];
        memset(file, 0, sizeof(file));
        for (; index < len; ++index)
            if (cmd[index] != ' ') break;
        for (int i = index; i < index + 3; ++i) str[i] = cmd[i];
        if (strcmp("cat", str) == 0) {
            int tmp = index += 3;
            while (cmd[tmp] == ' ' && tmp < len) ++tmp;
            if (tmp != index + 3) {
                int i = tmp;
                for (i = tmp; i < len; ++i) file[i - tmp] = cmd[i];
                showFileConent(window, file);
            } else
                sprintf(buf, "\ncmd error\n");
        } else
            sprintf(buf, "\ncmd error\n");
    }
    windowPutString(window, buf);
}

static void showFile(ConsleWindow* window) {
    FileInfo info[10];
    unsigned size = 0;
    getFileList(info, &size);
    char buf[50];
    windowPutChar(window, '\n');
    for (int i = 0; i < size; ++i) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "filename.ext   %7d\n", info[i].size);
        for (int j = 0; j < 8; ++j) buf[j] = info[i].name[j];
        buf[9] = info[i].ext[0];
        buf[10] = info[i].ext[1];
        buf[11] = info[i].ext[2];
        windowPutString(window, buf);
    }
}

static void windowPutChar(ConsleWindow* window, char ch) {
    int chx = window->curx, chy = window->cury;
    int x = chx, y = chy;
    Sheet* sheet = getWindowSheet(window);
    int wide = sheet->info.wide;
    if (ch == KEY_TAB || ch == '\r') {
        return;
    } else if (ch == KEY_BACKSPACE) {
        x = x - FONT_WIDE;
        if (x <= WINDOW_FONT_SIDE + FONT_WIDE) {
            x = y == WINDOW_TITLE_HIGH
                    ? WINDOW_FONT_SIDE + FONT_WIDE
                    : wide - (wide - WINDOW_FONT_SIDE * 2) % FONT_WIDE -
                          FONT_WIDE;
            y = y == WINDOW_TITLE_HIGH ? y : y - FONT_HIGH;
        }
    } else if (ch == KEY_ENTER) {
        x = WINDOW_FONT_SIDE;
        if (y + FONT_HIGH >= sheet->info.high - FONT_HIGH) {
            scrollWindow(window);
            drawRect(sheet, chx, chy - FONT_HIGH, FONT_WIDE, FONT_HIGH,
                     COLOR_GRAY);
        } else
            y += FONT_HIGH;
    } else {
        x += FONT_WIDE;
        if (x >= wide - 2 * WINDOW_FONT_SIDE - FONT_WIDE) {
            x = WINDOW_FONT_SIDE;
            if (y + FONT_HIGH >= sheet->info.high - FONT_HIGH) {
                scrollWindow(window);
                drawRect(sheet, chx, chy - FONT_HIGH, FONT_WIDE, FONT_HIGH,
                         COLOR_GRAY);
            } else
                y += FONT_HIGH;
        };
    }
    window->curx = x;
    window->cury = y;
    drawRect(sheet, chx, chy, FONT_WIDE, FONT_HIGH, COLOR_GRAY);
    if (ch != KEY_BACKSPACE && ch != KEY_ENTER)
        putChar(ch, sheet, chx, chy, COLOR_BLACK);
    windowRefresh(window, chx, chy, FONT_WIDE, FONT_HIGH);
}

static void windowPutString(ConsleWindow* window, const char* str) {
    char* pStr = str;
    int limit = strlen(str);
    for (int i = 0; i < limit; ++i) {
        windowPutChar(window, str[i]);
    }
}

static void scrollWindow(ConsleWindow* window) {
    Sheet* sheet = getWindowSheet(window);
    const unsigned wide = sheet->info.wide;
    const unsigned high = sheet->info.high - FONT_HIGH - 1;
    const unsigned tmpp = (FONT_HIGH)*wide;
    for (int i = WINDOW_TITLE_HIGH; i < high; ++i) {
        const unsigned tmp = i * wide;
        for (int j = 0; j < wide; ++j)
            sheet->buf[tmp + j] = sheet->buf[tmp + tmpp + j];
    }
    drawRect(sheet, 0, high, wide, FONT_HIGH, COLOR_GRAY);
    windowRefresh(window, 0, 0, wide, sheet->info.high);
}

static showFileConent(ConsleWindow* window, const char* filename) {
    FileInfo info;
    if (!getFileInfo(filename, &info)) {
        windowPutString(window, "\nfile can't be find\n");
        return;
    }
    char* buf = memoryAlloc4K(info.size);
    unsigned size = 0;
    getFileContent(filename, buf, &size);
    if (buf != NULL)
        for (int i = 0; i < size; ++i) {
            windowPutChar(window, buf[i]);
        }
    memoryFree4K(buf, info.size);
}
