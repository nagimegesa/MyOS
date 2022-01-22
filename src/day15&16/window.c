#include "window.h"

#include "graphic.h"
#include "memory.h"
#include "timer.h"
static void writableWindowChangeHeight(Sheet* sheet, int height);
BaseWindow* makeBaseWindow(const char* title, int x, int y, unsigned wide,
                           unsigned high) {
    static char closebtn[14][16] = {
        "OOOOOOOOOOOOOOO@", "OQQQQQQQQQQQQQ$@", "OQQQQQQQQQQQQQ$@",
        "OQQQ@@QQQQ@@QQ$@", "OQQQQ@@QQ@@QQQ$@", "OQQQQQ@@@@QQQQ$@",
        "OQQQQQQ@@QQQQQ$@", "OQQQQQ@@@@QQQQ$@", "OQQQQ@@QQ@@QQQ$@",
        "OQQQ@@QQQQ@@QQ$@", "OQQQQQQQQQQQQQ$@", "OQQQQQQQQQQQQQ$@",
        "O$$$$$$$$$$$$$$@", "@@@@@@@@@@@@@@@@"};
    BaseWindow* window = memoryAlloc(sizeof(BaseWindow));
    window->sheet = getEmptySheet();
    SheetInfo info;
    info.high = high < WINDOW_MIN_HIGH ? WINDOW_MIN_HIGH : high;
    info.wide = wide < WINDOW_MIN_WIDE ? WINDOW_MIN_WIDE : wide;
    info.height = -1;
    info.posx = x;
    info.posy = y;
    info.containerType = CONTAINER_BASE_WINDOW;
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
    return window;
}
WritableWindow* makeWriteableWindow(const char* title, int x, int y,
                                    unsigned wide, unsigned high) {
    WritableWindow* window = memoryAlloc(sizeof(WritableWindow));
    window->window = makeBaseWindow(title, x, y, wide, high);
    window->window->sheet->info.containerType = CONTAINER_WRITABLE_WINDOW;
    window->chx = 0;
    window->chy = WINDOW_TITLE_HIGH;
}
