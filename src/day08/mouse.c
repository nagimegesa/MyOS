// 初始化鼠标
#include "mouse.h"

#include <stdio.h>

#include "BaseFunction.h"
#include "buf.h"
#include "graphic.h"

struct Mouse {
    int x, y;
    struct Screen *screen;
    char model[16][16];
};

struct MouseBuf {
    struct FIFOBuf buf;
};

struct MouseStat {
    char btn;
    int offsetx, offsety;
};

struct Mouse *getMouse() {
    static struct Mouse mouse;
    return &mouse;
}

struct MouseStat *getMouseStat() {
    static struct MouseStat stat;
    return &stat;
};

void initMouse(struct Mouse *mouse, struct Screen *screen) {
    static char cursor[16][16] = {
        "**************..", "*OOOOOOOOOOO*...", "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....", "*OOOOOOOO*......", "*OOOOOOO*.......",
        "*OOOOOOO*.......", "*OOOOOOOO*......", "*OOOO**OOO*.....",
        "*OOO*..*OOO*....", "*OO*....*OOO*...", "*O*......*OOO*..",
        "**........*OOO*.", "*..........*OOO*", "............*OO*",
        ".............***"};
    mouse->screen = screen;
    mouse->x = screen->wide / 2;
    mouse->y = screen->high / 2;
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            if (cursor[i][j] == '*') {
                mouse->model[i][j] = COLOR_BLACK;
            } else if (cursor[i][j] == 'O') {
                mouse->model[i][j] = COLOR_WHITE;
            } else {
                mouse->model[i][j] = -1;
            }
        }
    }
    return;
}

// 绘制鼠标
void drawMouse(struct Mouse *mouse) {
    const int x = mouse->x, y = mouse->y;
    char *addr = mouse->screen->startAddr;
    const unsigned wide = mouse->screen->wide;
    drawRect(*mouse->screen, mouse->x, mouse->y, MOUSE_WIDE, MOUSE_HIGH,
             mouse->screen->color);
    for (int i = 0; i < 16; ++i) {
        const unsigned tmp = (y + i) * wide + x;
        for (int j = 0; j < 16; ++j) {
            if (mouse->model[i][j] != (char)-1) {
                addr[tmp + j] = mouse->model[i][j];
            }
        }
    }
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

const char isMouseBufMax(struct MouseBuf *buf) { return isFIFOMax(&buf->buf); }
const unsigned char mouseBufPop(struct MouseBuf *buf) {
    return FIFOPop(&buf->buf);
}
const char isMouseBufEmpty(struct MouseBuf *buf) {
    return isFIFOEmpty(&buf->buf);
}
const char mouseBufPush(struct MouseBuf *buf, const unsigned char key) {
    return FIFOPush(&buf->buf, key);
}
struct MouseBuf *getMouseBuf() {
    static struct MouseBuf buf;
    return &buf;
}
void mouseBufInit(struct MouseBuf *mouseBuf) {
    static unsigned char buf[128];
    FIFOInit(&mouseBuf->buf, buf, 128);
}
const int getMouseBufLen(struct MouseBuf *mouseBuf) {
    return getBufLen(&mouseBuf->buf);
}

void changeMouseStat(struct MouseBuf *buf, struct Screen screen) {
    static char chBuf[3];
    static int mousePrase = -1, cnt = 0;
    unsigned char ch = mouseBufPop(buf);
    closeLock();
    if (mousePrase != -1) {
        chBuf[mousePrase++] = ch;
        if (mousePrase == 3) {
                struct MouseStat *stat = getMouseStat();
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

void dealMouseStatChange(struct Screen screen) {
    struct MouseStat *stat = getMouseStat();
    struct Mouse *mouse = getMouse();
    drawRect(screen, mouse->x, mouse->y, MOUSE_WIDE, MOUSE_HIGH, screen.color);
    mouse->x += stat->offsetx;
    mouse->y += stat->offsety;
    if (mouse->x <= 0) mouse->x = 0;
    if (mouse->x >= screen.wide - MOUSE_WIDE)
        mouse->x = screen.wide - MOUSE_WIDE;
    if (mouse->y <= 0) mouse->y = 0;
    if (mouse->y >= screen.high - MOUSE_HIGH)
        mouse->y = screen.high - MOUSE_HIGH;
    drawMouse(mouse);
}
