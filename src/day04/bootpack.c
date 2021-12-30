#include "osstd.h"

void OSMain(void) {
    initPalette();
    char *p = 0xa0000;
    drawRect(p, 320, 0, 0, 200, 300, COLOR_RED);
    for (;;) pause();
}