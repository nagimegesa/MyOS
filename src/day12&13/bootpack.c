#include <stdio.h>

#include "KeyBoard.h"
#include "dsctbl.h"
#include "graphic.h"
#include "interrupt.h"
#include "memory.h"
#include "mouse.h"
#include "timer.h"
#include "window.h"

void dealKeyBoardInterrupt(int data, Sheet* sheet);
void func(void);
void func2(void);
void OSMain(void) {
    initGdtIdt();
    initPic();
    closeLock();
    initPit();
    unsigned memSize = memtest(0x00400000, 0xbfffffff);
    memoryMangerInit(getMemoryManger());

    int tmp = 1;
    tmp = memoryFree(0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
    memoryFree(0x00400000, memSize - 0x00400000);
    TimerMangerInit(getTimerManger());
    initPalette();
    sheetMangerInit(getSheetManger());
    Sheet* desktop = initDesktop();
    FIFOBuf* msgBuf = getMsgBuf();
    msgBufInit();
    Sheet* mouse = initMouse();
    writePort8(PIC0_IMR, 0xf8); /* 开放PIC1和键盘中断(11111001) 以及定时器中断*/
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
    setTimer(500, func);
    setTimer(300, func2);
    for (;;) {
        if (!isFIFOEmpty(msgBuf)) {
            int data = FIFOPop(msgBuf);
            if (isMouseMsg(data)) {
                changeMouseStat(data);
                dealMouseStatChange(mouse);
            } else if (isKeyBoardMsg(data)) {
                dealKeyBoardInterrupt(data, desktop);
            }
        }
    }
    return;
}

void dealKeyBoardInterrupt(int data, Sheet* desktop) {
    setLock();
    unsigned char str[40];
    data = data - MSG_KEYBOARD_MIN;
    closeLock();
    sprintf(str, "%02x", data);
    putString(str, desktop, 10, 10, COLOR_WHITE);
}
void func(void) {
    putChar('A', getSheetManger()->pSheet[0], 30, 30, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0]);
}
void func2(void) {
    putChar('B', getSheetManger()->pSheet[0], 30, 30, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0]);
}