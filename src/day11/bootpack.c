#include <stdio.h>

#include "BaseFunction.h"
#include "KeyBoard.h"
#include "dsctbl.h"
#include "graphic.h"
#include "interrupt.h"
#include "memory.h"
#include "mouse.h"
#include "window.h"

void dealKeyBoardInterrupt(KeyBoardBuf* buf, Sheet* sheet);
void OSMain(void) {
    initGdtIdt();
    initPic();
    closeLock();

    unsigned memSize = memtest(0x00400000, 0xbfffffff);
    memoryMangerInit(getMemoryManger());
    int tmp = 1;
    tmp = memoryFree(0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
    memoryFree(0x00400000, memSize - 0x00400000);

    initPalette();
    sheetMangerInit(getSheetManger());
    Sheet* desktop = initDesktop();

    KeyBoardBuf* keyBoardBuf = getKeyBuf();
    keyBufInit(keyBoardBuf);

    MouseBuf* mouseBuf = getMouseBuf();
    mouseBufInit(mouseBuf);
    Sheet* mouse = initMouse();
    writePort8(PIC0_IMR, 0xf9); /* 开放PIC1和键盘中断(11111001) */
    writePort8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */

    keyBoardInit();
    initMouseDevice();
    char buf[40];

    sprintf(buf, "memory %dMB free: %dKb %d", memSize / 1024 / 1024,
            getFreeMemorySize() / 1024, tmp);
    putString(buf, desktop, 20, 20, COLOR_WHITE);

    Window* window = makeBaseWindow("title", 50, 50, 100, 100);
    sheetChangeHeight(window, 1);

    sheetRefresh(desktop);
    // sheetRefresh(mouse);
    for (;;) {
        setLock();
        if (!isKeyBufEmpty(keyBoardBuf))
            dealKeyBoardInterrupt(keyBoardBuf, desktop);
        else if (!isMouseBufEmpty(mouseBuf)) {
            changeMouseStat(mouseBuf);
            dealMouseStatChange(mouse);
        } else
            closeLockAndPause();
    }
    return;
}

void dealKeyBoardInterrupt(KeyBoardBuf* buf, Sheet* desktop) {
    setLock();
    unsigned int data = 0;
    unsigned char str[40];
    data = keyBufPop(buf);
    closeLock();
    sprintf(str, "%02x", data);
    putString(str, desktop, 10, 10, COLOR_WHITE);
}
