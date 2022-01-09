#include <stdio.h>

#include "BaseFunction.h"
#include "KeyBoard.h"
#include "dsctbl.h"
#include "graphic.h"
#include "interrupt.h"
#include "mouse.h"

void dealKeyBoardInterrupt(struct KeyBoardBuf* buf, struct Screen screen);
void OSMain(void) {
    initGdtIdt();
    initPic();
    closeLock();

    initPalette();
    struct Screen screen;
    initDesktop(&screen);
    drawDesktop(screen);

    struct KeyBoardBuf* keyBoardBuf = getKeyBuf();

    keyBufInit(keyBoardBuf);

    struct Mouse* mouse = getMouse();
    struct MouseBuf* mouseBuf = getMouseBuf();
    mouseBufInit(mouseBuf);
    initMouse(mouse, &screen);
    drawMouse(mouse);

    writePort8(PIC0_IMR, 0xf9); /* 开放PIC1和键盘中断(11111001) */
    writePort8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */

    keyBoardInit();
    initMouseDevice();
    char buf[10];
    unsigned memSize = memtest(0x00400000, 0xbfffffff);
    sprintf(buf, "%dMB", memSize / 1024 / 1024);
    putString(buf, screen, 20, 20, COLOR_WHITE);
    for (;;) {
        setLock();
        if (!isKeyBufEmpty(keyBoardBuf))
            dealKeyBoardInterrupt(keyBoardBuf, screen);
        else if (!isMouseBufEmpty(mouseBuf)) {
            changeMouseStat(mouseBuf, screen);
            dealMouseStatChange(screen);
        } else
            closeLockAndPause();
    }
    return;
}

void dealKeyBoardInterrupt(struct KeyBoardBuf* buf, struct Screen screen) {
    setLock();
    unsigned int data = 0;
    unsigned char str[40];
    data = keyBufPop(buf);
    closeLock();
    sprintf(str, "%02x", data);
    putString(str, screen, 10, 10, COLOR_WHITE);
}
