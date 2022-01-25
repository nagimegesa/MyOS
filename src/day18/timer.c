#include "timer.h"

#include "dsctbl.h"
#include "memory.h"
static int cmp(Timer data, Timer pNode);
static Timer* setTaskTimer(unsigned timeOut);
static Timer** getTaskTimer();
static void taskSleepMain();

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
    for (int i = 0; i < TASK_LEVEL_CNT; ++i) {
        (*manger)->level[i].runingCnt = 0;
        (*manger)->level[i].runNow = 0;
    }
    Task* task = getTask();
    if (task != NULL) {
        task->tag = TASK_RUNING;
        task->priority = 2;
        task->level = 0;
        taskAdd(task);
        changeTaskSub();
        setTR(task->GDTNum);
        setTaskTimer(task->priority);
    }
    Task* idle = getTask();
    idle->stat.esp = (int)memoryAlloc4K(64 * 1024) + 64 * 1024;
    idle->stat.eip = (int)&taskSleepMain;
    idle->stat.es = 1 * 8;
    idle->stat.cs = 2 * 8;
    idle->stat.ss = 1 * 8;
    idle->stat.ds = 1 * 8;
    idle->stat.fs = 1 * 8;
    idle->stat.gs = 1 * 8;
    taskRuning(idle, TASK_LEVEL_CNT - 1, 1);
    return task;
}
void taskRemove(Task* task) {
    TaskManger** manger = getTaskManger();
    TaskLevel* lev = &(*manger)->level[(*manger)->levelNow];
    int index = 0;
    for (; index < lev->runingCnt; ++index)
        if (lev->pTask[index] == task) break;
    --lev->runingCnt;
    if (index < lev->runNow) --lev->runNow;
    if (lev->runNow >= lev->runingCnt) lev->runNow = 0;
    task->tag = TASK_SLEEP;
    for (; index < lev->runingCnt; ++index)
        lev->pTask[index] = lev->pTask[index + 1];
}
void taskAdd(Task* task) {
    TaskManger** manger = getTaskManger();
    TaskLevel* lev = &(*manger)->level[(*manger)->levelNow];
    lev->pTask[lev->runingCnt++] = task;
    task->tag = taskRuning;
}

Task* getTaskNow() {
    TaskManger** manger = getTaskManger();
    TaskLevel* lev = &(*manger)->level[(*manger)->levelNow];
    return lev->pTask[lev->runNow];
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

void taskRuning(Task* task, int level, int priority) {
    if (level < 0) level = task->level;
    if (priority > 0) task->priority = priority;
    if (task->tag == TASK_RUNING && task->level != level) taskRemove(task);
    if (task->tag != TASK_RUNING) {
        task->level = level;
        taskAdd(task);
    }
    (*getTaskManger())->isNeedChange = 1;
}

void changeTaskSub() {
    TaskManger** manger = getTaskManger();
    int index = 0;
    for (; index < TASK_LEVEL_CNT; ++index)
        if ((*manger)->level[index].runingCnt > 0) break;
    (*manger)->levelNow = index;
    (*manger)->isNeedChange = 0;
}
void changeTask() {
    TaskManger** manger = getTaskManger();
    TaskLevel* lev = &(*manger)->level[(*manger)->levelNow];
    Task* taskNow = lev->pTask[lev->runNow];
    lev->runNow = lev->runNow + 1 == lev->runingCnt ? 0 : lev->runNow + 1;
    if ((*manger)->isNeedChange) {
        changeTaskSub();
        lev = &(*manger)->level[(*manger)->levelNow];
    }
    Task* task = lev->pTask[lev->runNow];
    setTaskTimer(task->priority);
    if (taskNow != task) farJmp(0, task->GDTNum);
}

void taskSleep(Task* task) {
    TaskManger** manger = getTaskManger();
    if (task->tag == TASK_RUNING) {
        Task* taskNow = getTaskNow();
        taskRemove(task);
        if (task == taskNow) {
            changeTaskSub();
            taskNow = getTaskNow();
            farJmp(0, taskNow->GDTNum);
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

static void taskSleepMain() {
    for (;;) pause();
}
