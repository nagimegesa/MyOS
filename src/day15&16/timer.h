#ifndef __TIMER_H__
#define __TIMER_H__
#include "buf.h"

#define TIMER_MAX_CNT 256
#define TIMER_CNT_MAX 0xffffffff
#define TASK_MAX_CNT 1000
#define TASK_START_GDT 3

#define TASK_UNUSE 0
#define TASK_USING 1
#define TASK_RUNING 2
#define TASK_SLEEP 3

typedef struct Timer {
    unsigned realTime;
    unsigned timeOut;
    FIFOBuf* buf;
} Timer;
Declare_List(Timer);

typedef struct TimerManger {
    unsigned timerCnt;
    unsigned nextTime;
    unsigned time;
    ListPointer(Timer) timerList;
} TimerManger;

typedef struct TaskStat {
    // 任务设置相关信息
    int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
    // 32位寄存器
    int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    // 16位寄存器
    int es, cs, ss, ds, fs, gs;
    // 任务设置相关
    int ldtr, iomap;
} TaskStat;

typedef struct Task {
    int GDTNum, tag;
    TaskStat stat;
} Task;

typedef struct TaskManger {
    int runingCnt, runNow;
    Task* pTask[TASK_MAX_CNT];
    Task task[TASK_MAX_CNT];
} TaskManger;

void timerMangerInit(TimerManger* manger);
TaskManger** getTaskManger();
Task* taskMangerInit();
Task* getTask();
void taskRuning(Task* task);
void taskSleep(Task* task);

TimerManger* getTimerManger();

Timer* setTimer(unsigned timerOut, FIFOBuf* buf);
FIFOBuf* getATimerBuf();
void timeOut();
void closeTimer();
void changeTask();
void initTaskTimer();
Timer** getTaskTimer();
#define TIME_BUF_SIZE 20
#endif
