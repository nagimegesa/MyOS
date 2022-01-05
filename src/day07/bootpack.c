#include <stdio.h>

#include "BaseFunction.h"
#include "KeyBoard.h"
#include "dsctbl.h"
#include "graphic.h"
#include "interrupt.h"
#include "mouse.h"

void dealKeyBoardInterrupt(struct KeyBoardBuf* buf, struct Screen);

void OSMain(void) {
    initGdtIdt();
    initPic();
    closeLock();

    initPalette();
    struct Screen screen;
    initScreen(&screen);
    drawScreen(screen);

    struct KeyBoardBuf* keyBoardBuf = getKeyBuf();
    keyBoardInit();
    keyBufInit(keyBoardBuf);

    struct Mouse* mouse = getMouse();
    struct MouseBuf* mouseBuf = getMouseBuf();
    MouseBufInit(mouseBuf);
    initMouse(mouse, &screen);
    drawMouse(mouse);

    putString("hello", screen, 20, 20, COLOR_LIGHT_BLUE);
    writePort8(PIC0_IMR, 0xf9); /* 开放PIC1和键盘中断(11111001) */
    writePort8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */
    for (;;) {
        if (!isKeyBufEmpty(keyBoardBuf))
            dealKeyBoardInterrupt(keyBoardBuf, screen);
        pause();
    }
    return;
}

void dealKeyBoardInterrupt(struct KeyBoardBuf* buf, struct Screen screen) {
    setLock();
    static int index = 20;
    unsigned char str[32] = {0}, top = 0;
    while (!isKeyBufEmpty(buf)) str[top++] = keyBufPop(buf);
    sprintf(str, "%x", str);
    putString(str, screen, 20, index, COLOR_RED);
    index += 10;
    closeLock();
}
