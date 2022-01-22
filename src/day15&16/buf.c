#include "buf.h"

#include "memory.h"
#include "timer.h"
void FIFOInit(FIFOBuf* FIFObuf, int* buf, int size, Task* task) {
    FIFObuf->head = 0;
    FIFObuf->tail = 0;
    FIFObuf->len = 0;
    FIFObuf->maxLen = size;
    FIFObuf->buf = buf;
    FIFObuf->task = task;
}

char FIFOPush(FIFOBuf* buf, int key) {
    if (buf->len < buf->maxLen) {
        buf->buf[buf->head++] = key;
        buf->head %= (buf->maxLen);
        ++buf->len;
        if (buf->task != NULL && buf->task->tag != TASK_RUNING)
            taskRuning(buf->task);
        return 1;
    }
    return 0;
}

const int FIFOPop(FIFOBuf* buf) {
    if (buf->len > 0) {
        int key = buf->buf[buf->tail++];
        buf->tail %= (buf->maxLen);
        --buf->len;
        return key;
    }
    return 0;
}

const char isFIFOEmpty(FIFOBuf* buf) { return buf->len == 0; }

const char isFIFOMax(FIFOBuf* buf) { return buf->len == buf->maxLen; }

const int getBufLen(FIFOBuf* buf) { return buf->len; }

FIFOBuf* getAFIFOBuf(unsigned size, Task* task) {
    FIFOBuf* Fbuf = memoryAlloc(sizeof(FIFOBuf));
    int* buf = memoryAlloc(sizeof(int) * size);
    FIFOInit(Fbuf, buf, MSG_BUF_SIZE, task);
    return Fbuf;
}

FIFOBuf* getMsgBuf() {
    static FIFOBuf buf;
    return &buf;
}

void MsgBufInit(Task* task) {
    int* buf = memoryAlloc(sizeof(int) * MSG_BUF_SIZE);
    FIFOInit(getMsgBuf(), buf, MSG_BUF_SIZE, task);
}

inline int isMouseMsg(int msg) {
    return (MSG_MOUSE_MIN <= msg && msg <= MSG_MOUSE_MAX);
}

inline int isKeyBoardMsg(int msg) {
    return (MSG_KEYBOARD_MIN <= msg && msg <= MSG_KEYBOARD_MAX);
}
