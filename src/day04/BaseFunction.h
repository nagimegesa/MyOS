#ifndef __BASEFUNCTION__H
#define __BASEFUNCTION__H

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

// 写端口
void writePort8(int port, int data);
void writePort16(int port, int data);
void writePort32(int port, int data);

// 读端口
void readPort8(int port);
void readPort16(int port);
void readPort32(int port);

#endif
