#include <stdio.h>

#include "KeyBoard.h"
#include "dsctbl.h"
#include "graphic.h"
#include "interrupt.h"
#include "memory.h"
#include "mouse.h"
#include "timer.h"
#include "window.h"

void cmdMain(void);
void funcA(void);
void funcB(void);
void funcC(void);
void funcNum(int num);
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

    WindowMangerInit();
    timerMangerInit(getTimerManger());
    FIFOBuf* timeBuf = getATimerBuf();
    Task* A = taskMangerInit();
    taskRuning(A, 0, 2);
    Task* cmdTask = getTask();
    cmdTask->stat.eip = (int)&cmdMain;
    cmdTask->stat.esp = (int)memoryAlloc4K(64 * 1024) + 64 * 1024 - 8;
    cmdTask->stat.es = 1 * 8;
    cmdTask->stat.cs = 2 * 8;
    cmdTask->stat.ss = 1 * 8;
    cmdTask->stat.ds = 1 * 8;
    cmdTask->stat.fs = 1 * 8;
    cmdTask->stat.gs = 1 * 8;
    taskRuning(cmdTask, 2, 3);
    initPalette();
    sheetMangerInit(getSheetManger());
    Sheet* desktop = initDesktop();
    FIFOBuf* msgBuf = getMsgBuf();
    msgBuf->task = A;
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

    sheetRefresh(desktop, 0, 0, desktop->info.wide, desktop->info.high);
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
            unsigned time = FIFOPop(timeBuf);
        } else {
            closeLock();
            taskSleep(A);
        }
        closeLock();
    }
    return;
}

void cmdMain(void) {
    TimerManger* timeManger = getTimerManger();
    FIFOBuf* timeBuf = getATimerBuf();
    setTimer(50, timeBuf);
    ConsleWindow window;
    makeConsleWindow(&window, "cmd", 100, 100, 500, 300);
    Sheet* sheet = getWindowSheet(&window);
    // sheetChangeHeight(sheet, SHEET_TOP);
    windowChangeHeight(&window, WINDOW_TOP, WINDOW_TYPE_CMD);
    windowRefresh(&window, 0, 0, sheet->info.wide, sheet->info.high);
    for (;;) {
        setLock();
        if (!isFIFOEmpty(timeBuf)) {
            unsigned time = FIFOPop(timeBuf);
            if (time == 50) {
                drawCursor(&window);
                setTimer(50, timeBuf);
            }
        } else {
            closeLock();
            pause();
        }
        closeLock();
    }
}

void funcA(void) {
    static int x = 30;
    putChar('A', getSheetManger()->pSheet[0], x, 30, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0], x, 30, FONT_WIDE, FONT_HIGH);
    x += 10;
}
void funcB(void) {
    static int x = 40;
    putChar('B', getSheetManger()->pSheet[0], x, 40, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0], x, 40, FONT_WIDE, FONT_HIGH);
    x += 10;
}
void funcC(void) {
    static int x = 10;
    putChar('C', getSheetManger()->pSheet[0], x, 50, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0], x, 50, FONT_WIDE, FONT_HIGH);
    x += 10;
}
void funcNum(int num) {
    static int x = 10;
    char buf[10];
    sprintf(buf, "%d", num);
    putString(buf, getSheetManger()->pSheet[0], x, 60, COLOR_BLUE);
    sheetRefresh(getSheetManger()->pSheet[0], x, 60, FONT_WIDE * 10, FONT_HIGH);
    x += FONT_WIDE * 5;
}