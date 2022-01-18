#include "KeyBoard.h"

#include "BaseFunction.h"

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