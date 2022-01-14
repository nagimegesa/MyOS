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
    info.height = 2;
    info.high = 16;
    info.wide = 16;
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

const char isMouseBufMax(MouseBuf *buf) { return isFIFOMax(&buf->buf); }
const unsigned char mouseBufPop(MouseBuf *buf) { return FIFOPop(&buf->buf); }
const char isMouseBufEmpty(MouseBuf *buf) { return isFIFOEmpty(&buf->buf); }
const char mouseBufPush(MouseBuf *buf, const unsigned char key) {
    return FIFOPush(&buf->buf, key);
}

MouseBuf *getMouseBuf() {
    static struct MouseBuf buf;
    return &buf;
}
void mouseBufInit(MouseBuf *mouseBuf) {
    static unsigned char buf[128];
    FIFOInit(&mouseBuf->buf, buf, 128);
}
const int getMouseBufLen(MouseBuf *mouseBuf) {
    return getBufLen(&mouseBuf->buf);
}

void changeMouseStat(MouseBuf *buf) {
    static char chBuf[3];
    static int mousePrase = -1, cnt = 0;
    unsigned char ch = mouseBufPop(buf);
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
    int x = mouse->info.posx + stat->offsetx;
    int y = mouse->info.posy + stat->offsety;
    const unsigned screenWide = getSheetManger()->wide;
    const unsigned screenHigh = getSheetManger()->high;
    if (x <= 0) x = 0;
    if (y <= 0) y = 0;
    if (x >= screenWide - MOUSE_WIDE) x = screenWide - MOUSE_WIDE;
    if (y >= screenHigh - MOUSE_HIGH) y = screenHigh - MOUSE_HIGH;
    sheetMove(mouse, x, y);
}
