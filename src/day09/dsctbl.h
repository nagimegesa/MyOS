#ifndef _DSCTBLH__
#define _DSCTBLH__

struct SegmentDescriptor {
    short limitLow, baseLow;
    char baseMid, accessRight;
    char limitHigh, baseHigh;
};

struct GateDescriptor {
    short offsetLow, selector;
    char dwCount, accessRight;
    short offsetHigh;
};

// 初始化gdt和idt
void initGdtIdt(void);
void setSegmdesc(struct SegmentDescriptor* sd, unsigned int limit, int base,
                 int ar);
void setGatedesc(struct GateDescriptor* gd, int offset, int selector, int ar);
void loadGdtr(int limit, int addr);
void loadIdtr(int limit, int addr);

void asmKeyBoardInterrupt(void);
void asmMouseInterrupt(void);
void asm_inthandler27(void);
// 键盘中断
void keyBoardInterrupt(int* esp);

// 鼠标中断
void mouseInterrupt(int* esp);

void inthandler27(int* esp);

unsigned memtest(unsigned start, unsigned end);

// 段表的一些设置设定
#define ADR_IDT 0x0026f800
#define LIMIT_IDT 0x000007ff
#define ADR_GDT 0x00270000
#define LIMIT_GDT 0x0000ffff
#define ADR_BOTPAK 0x00280000
#define LIMIT_BOTPAK 0x0007ffff
#define AR_DATA32_RW 0x4092
#define AR_CODE32_ER 0x409a
#define AR_INTGATE32 0x008e

#define EFLAGS_AC_BIT 0x00040000
#define CR0_CACHE_DISABLE 0x60000000

#endif
