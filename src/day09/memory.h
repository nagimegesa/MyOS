#ifndef __MEMORY_H__
#define __MEMORY_H__

struct MemoryManger;

struct MemoryManger* getMemoryManger();

// 初始内存管理模块
void memoryMangerInit(struct MemoryManger*);

// 分配内存空间
void* memoryAlloc(unsigned int size);

// 按4K大小分配空间
void* memoryAlloc4K(unsigned size);

// 释放内存空间 返回0成功, 1失败
int memoryFree(int addr, unsigned size);

// 按4k大小释放空间
int memoryFree4K(int addr, unsigned size);

// 获得可用内存空间数量
unsigned getFreeMemorySize();

// 测试内存大小
unsigned memtest(unsigned start, unsigned end);

#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000

// 管理内存的分块数量
#define MEMORY_MANGER_SIZE 4096
#endif
