#include "timer.h"

#include "dsctbl.h"
static int cmp(Timer data, Timer pNode);
static Timer* setTaskTimer(unsigned timeOut);
static Timer** getTaskTimer();

typedef TimerManger Manger;
void timerMangerInit(TimerManger* manger) {
    manger->timerCnt = 0;
    manger->nextTime = TIMER_CNT_MAX;
    manger->timerList = GetEmptyList(Timer)(cmp);
}

TimerManger* getTimerManger() {
    static TimerManger manger;
    return &manger;
}

void initTaskTimer() { setTaskTimer(2); }

Timer* setTimer(unsigned timerOut, FIFOBuf* buf) {
    Manger* manger = getTimerManger();
    Timer timer;
    timer.realTime = timerOut;
    timer.timeOut = manger->time + timerOut;
    timer.buf = buf;
    Timer* tm = ListInsert(Timer)(manger->timerList, timer);
    if (tm == NULL) return NULL;
    manger->nextTime =
        timer.timeOut < manger->nextTime ? timer.timeOut : manger->nextTime;
    ++manger->timerCnt;
    return tm;
}

void closeTimer(Timer timer) {
    Manger* manger = getTimerManger();
    ListErase(Timer)(manger->timerList, timer);
}

void timeOut() {
    Manger* manger = getTimerManger();
    ListPointer(Timer) list = manger->timerList;
    char isTaskTimer = 0;
    if (!ListEmpty(Timer)(list)) {
        ListNodePointer(Timer) pNode = ListGetFirst(Timer)(list);
        if (&pNode->data != *getTaskTimer()) {
            FIFOPush(pNode->data.buf, pNode->data.realTime);
        } else
            isTaskTimer = 1;
        ListErase(Timer)(list, pNode->data);
        manger->nextTime = (ListEmpty(Timer)(list))
                               ? TIMER_CNT_MAX
                               : ListGetFirst(Timer)(list)->data.timeOut;
    }
    if (isTaskTimer) changeTask();
}

FIFOBuf* getATimerBuf() {
    FIFOBuf* FifoBuf = memoryAlloc(sizeof(FIFOBuf));
    int* buf = memoryAlloc(TIME_BUF_SIZE * sizeof(int));
    FIFOInit(FifoBuf, buf, TIME_BUF_SIZE, 0);
    return FifoBuf;
}

TaskManger** getTaskManger() {
    static TaskManger* manger;
    return &manger;
}

Task* taskMangerInit() {
    TaskManger** manger = getTaskManger();
    *manger = memoryAlloc4K(sizeof(TaskManger));
    SegmentDescriptor* gdt = (SegmentDescriptor*)ADR_GDT;
    for (int i = 0; i < TASK_MAX_CNT; ++i) {
        (*manger)->task[i].tag = TASK_UNUSE;
        (*manger)->task[i].GDTNum = (TASK_START_GDT + i) * 8;
        setSegmdesc(gdt + TASK_START_GDT + i, 103, &(*manger)->task[i].stat,
                    AR_TSS32);
    }
    Task* task = getTask();
    if (task != NULL) {
        task->tag = TASK_RUNING;
        (*manger)->runNow = 0;
        (*manger)->runingCnt = 1;
        (*manger)->pTask[0] = task;
        setTR(task->GDTNum);
        setTaskTimer(2);
    }
    return task;
}

Task* getTask() {
    TaskManger** manger = getTaskManger();
    for (int i = 0; i < TASK_MAX_CNT; ++i) {
        if ((*manger)->task[i].tag == TASK_UNUSE) {
            Task* task = &(*manger)->task[i];
            task->tag = TASK_USING;
            task->stat.eflags = 0x00000202; /* IF = 1; */
            task->stat.eax = 0;             /*这里先置为0*/
            task->stat.ecx = 0;
            task->stat.edx = 0;
            task->stat.ebx = 0;
            task->stat.ebp = 0;
            task->stat.esi = 0;
            task->stat.edi = 0;
            task->stat.es = 0;
            task->stat.ds = 0;
            task->stat.fs = 0;
            task->stat.gs = 0;
            task->stat.ldtr = 0;
            task->stat.iomap = 0x40000000;
            return task;
        }
    }
    return NULL;
}

void taskRuning(Task* task) {
    TaskManger** manger = getTaskManger();
    (*manger)->pTask[(*manger)->runingCnt++] = task;
    task->tag = TASK_RUNING;
}

void changeTask() {
    TaskManger** manger = getTaskManger();
    setTaskTimer(2);
    if ((*manger)->runingCnt >= 2) {
        (*manger)->runNow = (*manger)->runNow + 1 == (*manger)->runingCnt
                                ? 0
                                : (*manger)->runNow + 1;
        farJmp(0, (*manger)->pTask[(*manger)->runNow]->GDTNum);
    }
}

void taskSleep(Task* task) {
    TaskManger** manger = getTaskManger();
    if (task->tag == TASK_RUNING) {
        task->tag = TASK_SLEEP;
        char isNeedChange = 0;
        if ((*manger)->pTask[((*manger)->runNow)] == task) isNeedChange = 1;
        int index = 0;
        for (; index < (*manger)->runingCnt; ++index)
            if ((*manger)->pTask[index] == task) break;
        --((*manger)->runingCnt);
        if (index < (*manger)->runNow) --((*manger)->runNow);
        for (; index < (*manger)->runingCnt; ++index)
            (*manger)->pTask[index] = (*manger)->pTask[index + 1];
        if (isNeedChange) {
            if ((*manger)->runNow >= (*manger)->runingCnt)
                (*manger)->runNow = 0;
            farJmp(0, (*manger)->pTask[(*manger)->runNow]->GDTNum);
        }
    }
}

Define_List(Timer);

static Timer** getTaskTimer() {
    static Timer* timer = NULL;
    return &timer;
}

static Timer* setTaskTimer(unsigned timeOut) {
    Manger* manger = getTimerManger();
    Timer timer;
    timer.realTime = timeOut;
    timer.timeOut = manger->time + timeOut;
    timer.buf = NULL;
    Timer* tm = ListInsert(Timer)(manger->timerList, timer);
    if (tm == NULL) return NULL;
    manger->nextTime =
        timer.timeOut < manger->nextTime ? timer.timeOut : manger->nextTime;
    ++manger->timerCnt;
    *getTaskTimer() = tm;
    return tm;
}
static int cmp(Timer data, Timer pNode) { return data.timeOut < pNode.timeOut; }
