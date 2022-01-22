#include <stdio.h>

#include "KeyBoard.h"
#include "dsctbl.h"
#include "graphic.h"
#include "interrupt.h"
#include "memory.h"
#include "mouse.h"
#include "timer.h"
#include "window.h"

void taskMain(void);
void dealKeyBoardInterrupt(int data, Sheet* sheet);
void func(void);
void func2(void);
void func3(void);
void OSMain(void) {
    initGdtIdt();
    initPic();
    closeLock();
    unsigned memSize = memtest(0x00400000, 0xbfffffff);
    memoryMangerInit(getMemoryManger());

    int tmp = 1;
    tmp = memoryFree(0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
    memoryFree(0x00400000, memSize - 0x00400000);
    initPit();

    timerMangerInit(getTimerManger());
    FIFOBuf* timeBuf = getATimerBuf();
    Task* A = taskMangerInit();
    Task* B = getTask();
    B->stat.eip = (int)&taskMain;
    B->stat.esp = (int)memoryAlloc4K(64 * 1024) + 64 * 1024 - 8;
    B->stat.es = 1 * 8;
    B->stat.cs = 2 * 8;
    B->stat.ss = 1 * 8;
    B->stat.ds = 1 * 8;
    B->stat.fs = 1 * 8;
    B->stat.gs = 1 * 8;
    taskRuning(B);

    initPalette();
    sheetMangerInit(getSheetManger());
    Sheet* desktop = initDesktop();
    FIFOBuf* msgBuf = getMsgBuf();
    MsgBufInit(NULL);
    Sheet* mouse = initMouse();
    writePort8(PIC0_IMR, 0xf8); /* 开放PIC1和键盘中断(11111001) 以及定时器中断*/
    writePort8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */
    keyBoardInit();
    initMouseDevice();
    char buf[40];

    sprintf(buf, "memory %dMB free: %dKb %d", memSize / 1024 / 1024,
            getFreeMemorySize() / 1024, tmp);
    putString(buf, desktop, 20, 20, COLOR_WHITE);

    BaseWindow* window = makeBaseWindow("2", 50, 50, 100, 100);
    sheetChangeHeight(window->sheet, 1);
    BaseWindow* window2 = makeBaseWindow("3", 60, 60, 100, 100);
    sheetChangeHeight(window2->sheet, 2);
    sheetRefresh(desktop);
    msgBuf->task = A;
    for (;;) {
        setLock();
        if (!isFIFOEmpty(msgBuf)) {
            int data = FIFOPop(msgBuf);
            if (isMouseMsg(data)) {
                changeMouseStat(data);
                dealMouseStatChange(mouse);
            } else if (isKeyBoardMsg(data)) {
                dealKeyBoardInterrupt(data, desktop);
            }
        } else if (!isFIFOEmpty(timeBuf)) {
            // int time = FIFOPop(timeBuf);
            // if (time == 2) {
            //   farJmp(0, 4 * 8);
            //    setTimer(2, timeBuf);
            // }
        } else {
            taskSleep(A);
            //closeLock();
        }
        closeLock();
    }
    return;
}

void taskMain(void) {
    TimerManger* timeManger = getTimerManger();
    FIFOBuf* timeBuf = getATimerBuf();
    for (;;) {
        // if (!isFIFOEmpty(timeBuf)) {
        //    unsigned time = FIFOPop(timeBuf);
        //   if (time == 2) {
        //       farJmp(0, 3 * 8);
        // setTimer(2, timeBuf);
        ///   }
        //} else
        //     pause();
    }
}
void func(void) {
    static int x = 30;
    putChar('A', getSheetManger()->pSheet[0], x, 30, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0]);
    x += 10;
}
void func2(void) {
    static int x = 40;
    putChar('B', getSheetManger()->pSheet[0], x, 40, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0]);
    x += 10;
}
void func3(void) {
    putChar('C', getSheetManger()->pSheet[0], 10, 10, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0]);
}