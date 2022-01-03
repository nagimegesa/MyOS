#include "BaseFunction.h"
#include "dsctbl.h"
#include "graphic.h"
#include "interrupt.h"

void OSMain(void) {
    initGdtIdt();
    initPic();
    closeLock();

    initPalette();
    struct Screen screen;
    initScreen(&screen);
    drawScreen(screen);

    struct Mouse mouse;
    initMouse(&mouse, &screen);
    drawMouse(mouse);

    putString("hello", screen, 20, 20, COLOR_LIGHT_BLUE);

    writePort8(PIC0_IMR, 0xf9); /* 开放PIC1和键盘中断(11111001) */
    writePort8(PIC1_IMR, 0xef); /* 开放鼠标中断(11101111) */
    for (;;) pause();
    return;
}
