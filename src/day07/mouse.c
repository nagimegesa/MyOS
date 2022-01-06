// 初始化鼠标
#include "mouse.h"

#include "BaseFunction.h"
#include "buf.h"
#include "graphic.h"

struct Mouse {
    int x, y;
    struct Screen *screen;
    char **model;
};

struct MouseBuf {
    struct FIFOBuf buf;
};

struct Mouse *getMouse() {
    static struct Mouse mouse;
    return &mouse;
}

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
    mouse->model = cursor;
    return;
}

// 绘制鼠标
void drawMouse(const struct Mouse *mouse) {
    const int x = mouse->x, y = mouse->y;
    char *addr = mouse->screen->startAddr;
    const unsigned wide = mouse->screen->wide;
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
    static char buf[128];
    FIFOInit(&mouseBuf->buf, buf, 128);
}
