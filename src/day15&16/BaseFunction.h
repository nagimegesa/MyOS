#ifndef __BASEFUNCTION_H__
#define __BASEFUNCTION_H__

#ifndef DEFINE_NULL
#define DEFINE_NULL
#define NULL ((void*)0)
#endif

#ifndef DEFINE_SIZE_T
#define DEFINE_SIZE_T
typedef unsigned size_t;
#endif

// 休眠
void pause(void);

// 向指定地址写入数据
void writeToMemory(int addr, int data);

// 设置禁止中断
void setLock(void);

// 关闭禁止中断
void closeLock(void);

// 关闭禁止中断后休眠
void closeLockAndPause(void);

// 获得中断状态
int getLockTag(void);

// 设置中断状态
void setLockTag(int eflags);

// 获取CR0寄存器
int getCR0(void);

// 设置CR0寄存器
void setCR0(int data);

unsigned memoryTestSub(unsigned start, unsigned end);

// 写端口
void writePort8(int port, int data);
void writePort16(int port, int data);
void writePort32(int port, int data);

// 读端口
int readPort8(int port);
int readPort16(int port);
int readPort32(int port);

// 将指定段上限为limit和addr赋值给段寄存器
void setGdtr(int limit, int addr);

void loadGdtr(int limit, int addr);
void loadIdtr(int limit, int addr);

void asmKeyBoardInterrupt(void);
void asmMouseInterrupt(void);
void asmTimerInterrupt(void);
void asm_inthandler27(void);

void setTR(int tr);
void farJmp(int eip, int cs);

#endif
