#include <stdio.h>

#include "BaseFunction.h"
#include "KeyBoard.h"
#include "dsctbl.h"
#include "graphic.h"
#include "interrupt.h"
#include "mouse.h"

void dealKeyBoardInterrupt(struct KeyBoardBuf* buf, struct Screen screen);
void dealMouseInterrupt(struct MouseBuf* buf, struct Screen screen);
void OSMain(void) {
    initGdtIdt();
    initPic();
    closeLock();

    initPalette();
    struct Screen screen;
    initScreen(&screen);
    drawScreen(screen);

    struct KeyBoardBuf* keyBoardBuf = getKeyBuf();

    keyBufInit(keyBoardBuf);

    struct Mouse* mouse = getMouse();
    struct MouseBuf* mouseBuf = getMouseBuf();
    mouseBufInit(mouseBuf);
    initMouse(mouse, &screen);

    drawMouse(mouse);

    putString("hello", screen, 20, 20, COLOR_LIGHT_BLUE);
    writePort8(PIC0_IMR, 0xf9); /* 开放PIC1和键盘中断(11111001) */
    writePort8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */

    keyBoardInit();
    initMouseDevice();
    for (;;) {
        if (!isKeyBufEmpty(keyBoardBuf))
            dealKeyBoardInterrupt(keyBoardBuf, screen);
        else if (!isMouseBufEmpty(mouseBuf))
            dealMouseInterrupt(mouseBuf, screen);
        pause();
    }
    return;
}

void dealKeyBoardInterrupt(struct KeyBoardBuf* buf, struct Screen screen) {
    setLock();
    unsigned int data = 0;
    unsigned char str[40];
    while (!isKeyBufEmpty(buf)) {
        data = keyBufPop(buf);
        sprintf(str, "%x", data);
        drawRect(screen, 20, 20, 40, 20, COLOR_LIGHT_DARK_BLUE);
        putString(str, screen, 20, 20, COLOR_RED);
    }
    closeLock();
}

void dealMouseInterrupt(struct MouseBuf* buf, struct Screen screen) {
    setLock();
    unsigned int data = 0;
    unsigned char str[40];
    while (!isMouseBufEmpty(buf)) {
        data = mouseBufPop(buf);
        sprintf(str, "%x", data);
        drawRect(screen, 40, 40, 40, 20, COLOR_LIGHT_DARK_BLUE);
        putString(str, screen, 40, 40, COLOR_RED);
    }
    closeLock();
}
