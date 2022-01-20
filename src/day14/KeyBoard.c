#include "KeyBoard.h"

#include "BaseFunction.h"
#include "graphic.h"
void keyBoardInit() {
    waitKeyBoardReady();
    writePort8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
    waitKeyBoardReady();
    writePort8(PORT_KEYDAT, KBC_MODE);
}

void waitKeyBoardReady() {
    for (;;)
        if ((readPort8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) return;
}

void dealKeyBoardInterrupt(int data, Sheet* sheet) {
#define X 200
#define Y 200
    static char keytable[0x54] = {
        0, 0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^',
        0, 0,   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[',
        0, 0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,
        0, ']', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0,   '*',
        0, ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
        0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'};
    char ch = keytable[data - MSG_KEYBOARD_MIN];
    if (ch != 0) {
        drawRect(sheet, X, Y, FONT_WIDE, FONT_HIGH, DESKTOP_COLOR);
        putChar(ch, sheet, X, Y, COLOR_WHITE);
        sheetRefreshSub(X, Y, FONT_WIDE, FONT_HIGH, sheet->info.height);
    }
}
