#include "KeyBoard.h"

#include "BaseFunction.h"
#include "buf.h"

struct KeyBoardBuf {
    struct FIFOBuf buf;
};

const char isKeyBufEmpty(struct KeyBoardBuf* buf) {
    return isFIFOEmpty(&buf->buf);
}
const char isKeyBufMax(struct KeyBoardBuf* buf) { return isFIFOMax(&buf->buf); }
const unsigned char keyBufPop(struct KeyBoardBuf* buf) {
    return FIFOPop(&buf->buf);
}

const char keyBufPush(struct KeyBoardBuf* buf, const unsigned char key) {
    return FIFOPush(&buf->buf, key);
}

void keyBufInit(struct KeyBoardBuf* buf) {
    static unsigned char keyBuf[32];
    FIFOInit(&buf->buf, keyBuf, 32);
}

struct KeyBoardBuf* getKeyBuf() {
    static struct KeyBoardBuf buf;
    return &buf;
}

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