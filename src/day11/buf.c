#include "buf.h"

void FIFOInit(FIFOBuf* FIFObuf, char* buf, int size) {
    for (int i = 0; i < size; ++i) FIFObuf->buf[i] = 0x00;
    FIFObuf->head = 0;
    FIFObuf->tail = 0;
    FIFObuf->len = 0;
    FIFObuf->maxLen = size;
    FIFObuf->buf = buf;
}

char FIFOPush(FIFOBuf* buf, const unsigned char key) {
    if (buf->len < buf->maxLen) {
        buf->buf[buf->head++] = key;
        buf->head %= (buf->maxLen);
        ++buf->len;
        return 1;
    }
    return 0;
}

const unsigned char FIFOPop(FIFOBuf* buf) {
    if (buf->len > 0) {
        unsigned char key = buf->buf[buf->tail++];
        buf->tail %= (buf->maxLen);
        --buf->len;
        return key;
    }
    return 0;
}

const char isFIFOEmpty(FIFOBuf* buf) { return buf->len == 0; }

const char isFIFOMax(FIFOBuf* buf) { return buf->len == buf->maxLen; }

const int getBufLen(FIFOBuf* buf) { return buf->len; }