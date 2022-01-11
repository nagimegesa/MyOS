#ifndef __BASEFUNCTION_H__
#define __BASEFUNCTION_H__

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
char readPort8(int port);
char readPort16(int port);
char readPort32(int port);

// 将指定段上限为limit和addr赋值给段寄存器
void setGdtr(int limit, int addr);

void loadGdtr(int limit, int addr);
void loadIdtr(int limit, int addr);

void asmKeyBoardInterrupt(void);
void asmMouseInterrupt(void);
void asm_inthandler27(void);

#endif
