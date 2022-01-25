#include "KeyBoard.h"

#include "BaseFunction.h"
#include "graphic.h"
#include "window.h"
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
/**
 * esc 1
 * r ctrl 29
 * tab 15
 * l ctrl 41
 * r shift 42
 * enter 28
 * l alt 56
 * r atl 224 56
 * space 56
 * capslock 58
 * F1 - F10 59 - 67
 * F11 - F12 87 -88
 * delete 224 224
 */

#define __KEY_ESC 1
#define __KEY_TAB 15
#define __KEY_BACKSPACE 14
#define __KEY_ENTER 28
#define __KEY_RIGHT_CTRL 29
#define __KEY_LEFT_SHIFT 41
#define __KEY_RIGHT_SHIFT 42
#define __KEY_LEFT_ATL 56
#define __KEY_CAPSLOCK 58

void dealKeyBoardInterrupt(int data, Sheet* sheet) {
    static char keytable0[60] = {
        0,   0,   '1', '2', '3',  '4', '5', '6',  '7', '8', '9', '0',
        '-', '=', 0,   0,   'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
        'o', 'p', '[', ']', 0,    0,   'a', 's',  'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';', '\'', 0,   0,   '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.',  '/', 'q', '*',  0,   ' ', 0};
    static char keytable1[60] = {
        0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
        '_', '+', 0,   0,   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
        'O', 'P', '{', '}', 0,   0,   'A', 'S', 'D', 'F', 'G', 'H',
        'J', 'K', 'L', ':', '"', 0,   0,   '|', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0};
    static int stdShift = 0x0000;
    static int isShift = 0x0000;
    int num = data - MSG_KEYBOARD_MIN;
    char ch = (isShift ^ stdShift) ? keytable1[num] : keytable0[num];
    if (num > 60) return;
    if (ch == 0) {
        switch (num) {
            case __KEY_ESC:
                ch = 27;
                break;
            case __KEY_TAB:
                ch = 9;
                break;
            case __KEY_ENTER:
                ch = 13;
                break;
            case __KEY_BACKSPACE:
                ch = 8;
                break;
            case __KEY_CAPSLOCK:
                stdShift = !stdShift;
                break;
            case __KEY_LEFT_SHIFT:
            case __KEY_RIGHT_SHIFT:
                isShift = 1;
                break;
            default:
                break;
        }
    }
    if (ch != 0) {
        void* window = getTopWindow();
        if (window != NULL && getWindowType(window) == WINDOW_TYPE_CMD) {
            windowInput(window, ch);
            if (ch != __KEY_LEFT_SHIFT && ch != __KEY_RIGHT_SHIFT)
                isShift = 0x0000;
        }
    }
}
