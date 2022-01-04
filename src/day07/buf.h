#ifndef __BUF_H__
#define __BUF_H__

struct KeyBoardBuf {
    unsigned char buf[32];
    unsigned char head, tail, len;
};

extern struct KeyBoardBuf keyBoardBuf;
void keyBufInit();
const char isKeyBufEmpty();
const char isKeyBufMax();
void keyBufPush(const unsigned char key);
const unsigned char keyBufPop();

#endif