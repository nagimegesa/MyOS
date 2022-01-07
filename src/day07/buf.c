#include "buf.h"

void FIFOInit(struct FIFOBuf* FIFObuf, char* buf, int size) {
    for (int i = 0; i < 32; ++i) FIFObuf->buf[i] = 0x00;
    FIFObuf->head = 1;
    FIFObuf->tail = 0;
    FIFObuf->len = 0;
    FIFObuf->maxLen = size;
    FIFObuf->buf = buf;
}

char FIFOPush(struct FIFOBuf* buf, const unsigned char key) {
    if (buf->len <= buf->maxLen) {
        buf->buf[buf->head++] = key;
        buf->head %= buf->maxLen;
        ++buf->len;
        return 1;
    }
    return -1;
}

const unsigned char FIFOPop(struct FIFOBuf* buf) {
    if (buf->len >= 0) {
        unsigned char key = buf->buf[buf->tail++];
        buf->tail %= buf->maxLen;
        --buf->len;
        return key;
    }
    return 0;
}

const char isFIFOEmpty(struct FIFOBuf* buf) { return buf->len == 0; }

const char isFIFOMax(struct FIFOBuf* buf) { return buf->len == buf->maxLen; }