#include "memory.h"

#include "BaseFunction.h"
struct MemoryInfo {
    char* addr;
    unsigned int size;
};
typedef struct MemoryManger {
    unsigned freeSize;
    unsigned failedCnt;
    unsigned failedSize;
    struct MemoryInfo memInfoArr[MEMORY_MANGER_SIZE];
} Manger;

MemoryManger* getMemoryManger() {
    static struct MemoryManger memMan;
    return &memMan;
}

void memoryMangerInit(MemoryManger* memMan) {
    memMan->freeSize = 0;
    memMan->failedCnt = 0;
    memMan->failedSize = 0;
}

unsigned getFreeMemorySize() {
    unsigned size = 0;
    Manger* manger = getMemoryManger();
    for (int i = 0; i < manger->freeSize; ++i)
        size += manger->memInfoArr[i].size;
    return size;
}

void* memoryAlloc(unsigned int size) {
    Manger* manger = getMemoryManger();
    int index = 0;
    for (index = 0; index < MEMORY_MANGER_SIZE; ++index)
        if (manger->memInfoArr[index].size >= size) {
            void* addr = manger->memInfoArr[index].addr;
            manger->memInfoArr[index].addr += size;
            manger->memInfoArr[index].size -= size;
            if (manger->memInfoArr[index].size == 0) {
                unsigned limit = --manger->freeSize;
                for (; index < limit; ++index)
                    manger->memInfoArr[index] = manger->memInfoArr[index + 1];
            }
            return addr;
        }
    return NULL;
}
// 按4K大小分配空间
void* memoryAlloc4K(unsigned size) {
    size = (size + 0xfff) & 0xfffff000;
    return memoryAlloc(size);
}
// 按4k大小释放空间
int memoryFree4K(int addr, unsigned size) {
    size = (size + 0xfff) & 0xfffff000;
    return memoryFree(addr, size);
}

int memoryFree(int addr, unsigned size) {
    Manger* manger = getMemoryManger();
    int index = 0;
    for (; index < manger->freeSize; ++index)
        if (manger->memInfoArr[index].addr > addr) break;
    if (index > 0 && manger->memInfoArr[index - 1].addr +
                             manger->memInfoArr[index - 1].size ==
                         addr) {
        manger->memInfoArr[index - 1].size += size;
        if (index < manger->freeSize &&
            addr + size == manger->memInfoArr[index].addr) {
            manger->memInfoArr[index - 1].size +=
                manger->memInfoArr[index].size;
            unsigned limit = --manger->freeSize;
            for (; index < limit; ++index)
                manger->memInfoArr[index] = manger->memInfoArr[index + 1];
        }
        return 0;
    }
    if (index < manger->failedCnt &&
        addr + size == manger->memInfoArr[index].addr) {
        manger->memInfoArr[index].addr = addr;
        manger->memInfoArr[index].size += size;
        return 0;
    }
    if (manger->failedCnt < MEMORY_MANGER_SIZE) {
        for (int i = manger->freeSize; i > index; --i)
            manger->memInfoArr[i] = manger->memInfoArr[i - 1];
        ++manger->freeSize;
        manger->memInfoArr[index].addr = addr;
        manger->memInfoArr[index].size = size;
        return 0;
    }
    ++manger->failedCnt;
    manger->failedSize += size;
    return 1;
}

unsigned memtest(unsigned start, unsigned end) {
    char is486 = 0;
    unsigned lockTag = getLockTag();
    lockTag |= EFLAGS_AC_BIT;
    setLockTag(lockTag);
    lockTag = getLockTag();
    if (lockTag & EFLAGS_AC_BIT) is486 = 1;
    lockTag &= ~EFLAGS_AC_BIT;
    setLockTag(lockTag);
    if (is486) {
        char cr0Tag = getCR0();
        cr0Tag |= CR0_CACHE_DISABLE;
        setCR0(cr0Tag);
    }
    unsigned memSize = memoryTestSub(start, end);
    if (is486) {
        char cr0Tag = getCR0();
        cr0Tag &= ~CR0_CACHE_DISABLE;
        setCR0(cr0Tag);
    }
    return memSize;
}
