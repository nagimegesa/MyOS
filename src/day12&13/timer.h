#ifndef __TIMER_H__
#define __TIMER_H__

#define TIMER_MAX_CNT 256
#define TIMER_CNT_MAX 0xffffffff
#include "buf.h"

typedef struct Timer {
    unsigned timeOut;
    void (*func)();
} Timer;
Declare_List(Timer);

typedef struct TimerManger {
    unsigned timerCnt;
    unsigned nextTime;
    unsigned time;
    ListPointer(Timer) timerList;
} TimerManger;

void TimerMangerInit(TimerManger* manger);
TimerManger* getTimerManger();

Timer* setTimer(int timerOut, void (*func)());

void timeOut();
char closeTimer();
#endif
