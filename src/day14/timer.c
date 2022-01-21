#include "timer.h"

int cmp(Timer data, Timer pNode) { return data.timeOut < pNode.timeOut; }

typedef TimerManger Manger;
void TimerMangerInit(TimerManger* manger) {
    manger->timerCnt = 0;
    manger->nextTime = TIMER_CNT_MAX;
    int* buf = memoryAlloc(sizeof(int) * TIME_BUF_SIZE);
    FIFOInit(&manger->timerBuf, buf, TIME_BUF_SIZE);
    manger->timerList = GetEmptyList(Timer)(cmp);
}

TimerManger* getTimerManger() {
    static TimerManger manger;
    return &manger;
}

Timer* setTimer(int timerOut, void (*func)()) {
    Manger* manger = getTimerManger();
    Timer timer;
    timer.func = func;
    timer.timeOut = manger->time + timerOut;
    Timer* tm = ListInsert(Timer)(manger->timerList, timer);
    if (tm == NULL) return;
    manger->nextTime =
        timer.timeOut < manger->nextTime ? timer.timeOut : manger->nextTime;
    ++manger->timerCnt;
    return tm;
}

void timeOut() {
    Manger* manger = getTimerManger();
    ListPointer(Timer) list = manger->timerList;
    if (!ListEmpty(Timer)(list)) {
        ListNodePointer(Timer) pNode = ListGetFirst(Timer)(list);
        FIFOPush(&manger->timerBuf, (int)pNode->data.func);
        // pNode->data.func();
        ListErase(Timer)(list, pNode->data);
        manger->nextTime = (ListEmpty(Timer)(list))
                               ? TIMER_CNT_MAX
                               : ListGetFirst(Timer)(list)->data.timeOut;
    }
}

Define_List(Timer)