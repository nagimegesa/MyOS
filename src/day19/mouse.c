// 初始化鼠标
#include "mouse.h"

#include <stdio.h>

#include "BaseFunction.h"
#include "graphic.h"
#include "memory.h"

MouseStat *getMouseStat() {
    static struct MouseStat stat;
    return &stat;
};

Sheet *initMouse() {
    static char cursor[16][16] = {
        "**************..", "*OOOOOOOOOOO*...", "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....", "*OOOOOOOO*......", "*OOOOOOO*.......",
        "*OOOOOOO*.......", "*OOOOOOOO*......", "*OOOO**OOO*.....",
        "*OOO*..*OOO*....", "*OO*....*OOO*...", "*O*......*OOO*..",
        "**........*OOO*.", "*..........*OOO*", "............*OO*",
        ".............***"};
    Sheet *mouse = getEmptySheet();
    SheetInfo info;
    info.height = SHEET_HEIGHT_MAX - 1;
    info.high = MOUSE_HIGH;
    info.wide = MOUSE_WIDE;
    info.posx = getSheetManger()->wide / 2;
    info.posy = getSheetManger()->high / 2;
    char *model = memoryAlloc(sizeof(cursor));

    for (int i = 0; i < MOUSE_HIGH; ++i) {
        for (int j = 0; j < MOUSE_WIDE; ++j) {
            if (cursor[i][j] == '*') {
                model[i * MOUSE_WIDE + j] = COLOR_BLACK;
            } else if (cursor[i][j] == 'O') {
                model[i * MOUSE_WIDE + j] = COLOR_WHITE;
            } else {
                model[i * MOUSE_WIDE + j] = COLOR_NONE;
            }
        }
    }
    emptySheetInit(mouse, model, info);
    return mouse;
}

// 定义在KeyBoard.c
void waitKeyBoardReady();
void initMouseDevice() {
    /* 激活鼠标 */
    waitKeyBoardReady();
    writePort8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
    waitKeyBoardReady();
    writePort8(PORT_KEYDAT, MOUSECMD_ENABLE);
    return;
    /* 顺利的话,键盘控制其会返送回ACK(0xfa)*/
}

void changeMouseStat(int data) {
    static int chBuf[3];
    static int mousePrase = -1, cnt = 0;
    int ch = data - MSG_MOUSE_MIN;
    closeLock();
    if (mousePrase != -1) {
        chBuf[mousePrase++] = ch;
        if (mousePrase == 3) {
            MouseStat *stat = getMouseStat();
            mousePrase = 0;
            cnt = 0;
            stat->btn = chBuf[0] & 0x07;
            stat->offsetx = chBuf[1];
            stat->offsety = chBuf[2];
            if (chBuf[0] & 0x10) stat->offsetx |= 0xffffff00;
            if (chBuf[0] & 0x20) stat->offsety |= 0xffffff00;
            stat->offsety = -stat->offsety;
        }
    } else if (ch == 0xfa)
        mousePrase = 0;
}

void dealMouseStatChange(Sheet *mouse) {
    MouseStat *stat = getMouseStat();
    SheetManger *manger = getSheetManger();
    const int oldx = mouse->info.posx;
    const int oldy = mouse->info.posy;
    int x = oldx + stat->offsetx;
    int y = oldy + stat->offsety;
    const unsigned screenWide = manger->wide;
    const unsigned screenHigh = manger->high;
    x = x <= 0 ? 0 : x;
    x = x <= screenWide ? x : screenWide;
    y = y <= 0 ? 0 : y;
    y = y <= screenHigh ? y : screenHigh;
    if (stat->btn & MOUSE_BTN_LEFT) {
        const int index = oldy * screenWide + oldx - 1;
        if (index < manger->area) {
            const int tmp = manger->sheetMap[index];
            char buf[10];
            drawRect(manger->pSheet[0], 500, 500, FONT_WIDE, FONT_HIGH,
                     DESKTOP_COLOR);
            sprintf(buf, "%d", tmp);
            putString(buf, manger->pSheet[0], 500, 500, COLOR_RED);
            if (tmp > 1) {
                Sheet *sheet = &manger->sheet[tmp];
                if (x > 0 && x < screenWide && y > sheet->info.posy &&
                    y < sheet->info.posy + 16)
                    sheetMove(sheet, sheet->info.posx + stat->offsetx,
                              sheet->info.posy + stat->offsety);
                if (sheet->info.height < manger->cnt - 1)
                    sheetChangeHeight(sheet, SHEET_TOP);
            }
        }
    }
    sheetMove(mouse, x, y);
}
