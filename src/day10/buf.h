#ifndef __BUF_H__
#define __BUF_H__

typedef struct FIFOBuf {
    unsigned char* buf;
    unsigned char head, tail, len, maxLen;
} FIFOBuf;
void FIFOInit(FIFOBuf* FIFObuf, char* buf, int size);
const char isFIFOEmpty(FIFOBuf* buf);
const char isFIFOMax(FIFOBuf* buf);
char FIFOPush(FIFOBuf* buf, const unsigned char key);
const unsigned char FIFOPop(FIFOBuf* buf);
const int getBufLen(FIFOBuf* buf);

#endif