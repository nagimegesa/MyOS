#include "osstd.h"

void OSMain(void) {
    initPalette();
    struct Screen screen;
    initScreen(&screen);
    drawScreen(screen);
    putString("hello", screen, 20, 20, COLOR_LIGHT_BLUE);
    for (;;) pause();
    return;
}
