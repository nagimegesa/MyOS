#include "window.h"

#include "graphic.h"
#include "memory.h"

Window* makeBaseWindow(const char* title, int x, int y, unsigned wide,
                       unsigned high) {
    static char closebtn[14][16] = {
        "OOOOOOOOOOOOOOO@", "OQQQQQQQQQQQQQ$@", "OQQQQQQQQQQQQQ$@",
        "OQQQ@@QQQQ@@QQ$@", "OQQQQ@@QQ@@QQQ$@", "OQQQQQ@@@@QQQQ$@",
        "OQQQQQQ@@QQQQQ$@", "OQQQQQ@@@@QQQQ$@", "OQQQQ@@QQ@@QQQ$@",
        "OQQQ@@QQQQ@@QQ$@", "OQQQQQQQQQQQQQ$@", "OQQQQQQQQQQQQQ$@",
        "O$$$$$$$$$$$$$$@", "@@@@@@@@@@@@@@@@"};
    Sheet* sheet = getEmptySheet();
    SheetInfo info;
    info.high = high < WINDOW_MIN_HIGH ? WINDOW_MIN_HIGH : high;
    info.wide = wide < WINDOW_MIN_WIDE ? WINDOW_MIN_WIDE : wide;
    info.height = -1;
    info.posx = x;
    info.posy = y;
    char* buf = memoryAlloc(wide * high);
    emptySheetInit(sheet, buf, info);
    drawRect(sheet, 0, 0, wide, high, COLOR_GRAY);
    drawRect(sheet, 0, 0, wide, 14, COLOR_BLACK);
    putString(title, sheet, 0, 0, COLOR_WHITE);
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
    return sheet;
}
