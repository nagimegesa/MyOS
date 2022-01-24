#include "dsctbl.h"

#include "BaseFunction.h"

void initGdtIdt(void) {
    SegmentDescriptor* gdt = (SegmentDescriptor*)ADR_GDT;
    GateDescriptor* idt = (GateDescriptor*)ADR_IDT;

    /* GDT初始化 */
    for (int i = 0; i <= LIMIT_GDT / 8; i++) {
        setSegmdesc(gdt + i, 0, 0, 0);
    }
    setSegmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
    setSegmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
    loadGdtr(LIMIT_GDT, ADR_GDT);

    /* IDT初始化 */
    for (int i = 0; i <= LIMIT_IDT / 8; i++) {
        setGatedesc(idt + i, 0, 0, 0);
    }
    loadIdtr(LIMIT_IDT, ADR_IDT);

    /* IDT设置*/
    setGatedesc(idt + 0x20, (int)asmTimerInterrupt, 2 * 8, AR_INTGATE32);
    setGatedesc(idt + 0x21, (int)asmKeyBoardInterrupt, 2 * 8, AR_INTGATE32);
    setGatedesc(idt + 0x27, (int)asm_inthandler27, 2 * 8, AR_INTGATE32);
    setGatedesc(idt + 0x2c, (int)asmMouseInterrupt, 2 * 8, AR_INTGATE32);

    return;
}

void setSegmdesc(SegmentDescriptor* sd, unsigned int limit, int base, int ar) {
    if (limit > 0xfffff) {
        ar |= 0x8000; /* GBit = 1 */
        limit /= 0x1000;
    }
    sd->limitLow = limit & 0xffff;
    sd->baseLow = base & 0xffff;
    sd->baseMid = (base >> 16) & 0xff;
    sd->accessRight = ar & 0xff;
    sd->limitHigh = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
    sd->baseHigh = (base >> 24) & 0xff;
    return;
}

void setGatedesc(GateDescriptor* gd, int offset, int selector, int ar) {
    gd->offsetLow = offset & 0xffff;
    gd->selector = selector;
    gd->dwCount = (ar >> 8) & 0xff;
    gd->accessRight = ar & 0xff;
    gd->offsetHigh = (offset >> 16) & 0xffff;
    return;
}

void initPit() {
    writePort8(PIT_CTRL, 0x34);
    writePort8(PIT_CNT0, 0x9c);
    writePort8(PIT_CNT0, 0x2e);
}