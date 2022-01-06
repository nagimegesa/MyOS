#ifndef __BUF_H__
#define __BUF_H__

struct FIFOBuf {
    unsigned char* buf;
    unsigned char head, tail, len, maxLen;
};
void FIFOInit(struct FIFOBuf* FIFObuf, char* buf, int size);
const char isFIFOEmpty(struct FIFOBuf* buf);
const char isFIFOMax(struct FIFOBuf* buf);
char FIFOPush(struct FIFOBuf* buf, const unsigned char key);
const unsigned char FIFOPop(struct FIFOBuf* buf);

#endif