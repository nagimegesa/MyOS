#include "buf.h"

void FIFOInit(struct FIFOBuf* FIFObuf, char* buf, int size) {
    for (int i = 0; i < 32; ++i) FIFObuf->buf[i] = 0x00;
    FIFObuf->head = 1;
    FIFObuf->tail = 0;
    FIFObuf->len = 0;
    FIFObuf->maxLen = size;
    FIFObuf->buf = buf;
}

void FIFOPush(struct FIFOBuf* buf, const unsigned char key) {
    buf->buf[buf->head++] = key;
    buf->head %= buf->maxLen;
    ++buf->len;
}

const unsigned char FIFOPop(struct FIFOBuf* buf) {
    unsigned char key = buf->buf[buf->tail++];
    buf->tail %= buf->maxLen;
    --buf->len;
    return key;
}

const char isFIFOEmpty(struct FIFOBuf* buf) { return buf->len == 0; }

const char isFIFOMax(struct FIFOBuf* buf) { return buf->len == buf->maxLen; }