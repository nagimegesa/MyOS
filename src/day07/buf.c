#include "buf.h"

struct KeyBoardBuf keyBoardBuf;
void keyBufInit() {
    for (int i = 0; i < 32; ++i) keyBoardBuf.buf[i] = 0x00;
    keyBoardBuf.head = 1;
    keyBoardBuf.tail = 0;
    keyBoardBuf.len = 0;
}

void keyBufPush(const unsigned char key) {
    keyBoardBuf.buf[keyBoardBuf.head++] = key;
    keyBoardBuf.head %= 32;
    ++keyBoardBuf.len;
}

const unsigned char keyBufPop() {
    unsigned char key = keyBoardBuf.buf[keyBoardBuf.tail++];
    keyBoardBuf.tail %= 32;
    --keyBoardBuf.len;
    return key;
}

const char isKeyBufEmpty() {
    return keyBoardBuf.head % 32 == (keyBoardBuf.tail + 1) % 32;
}

const char isKeyBufMax() { return keyBoardBuf.len == 32; }