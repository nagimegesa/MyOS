#include "interrupt.h"

#include <stdio.h>

#include "BaseFunction.h"
#include "buf.h";
// 初始化PIC
void initPic(void) {
    writePort8(PIC0_IMR, 0xff);  // 禁止所有中断
    writePort8(PIC1_IMR, 0xff);  //禁止所有中断

    writePort8(PIC0_ICW1, 0x11);  //  边缘触发模式（edge trigger mode）
    writePort8(PIC0_ICW2, 0x20);  //  IRQ0-7由INT20-27接收
    writePort8(PIC0_ICW3, 1 << 2);  //  PIC1由IRQ2相连
    writePort8(PIC0_ICW4, 0x01);    //  无缓冲区模式

    writePort8(PIC1_ICW1, 0x11);  //  边缘触发模式（edge trigger mode）
    writePort8(PIC1_ICW2, 0x28);  //  IRQ8-15由INT28-2f接收
    writePort8(PIC1_ICW3, 2);     //  PIC1由IRQ2连接
    writePort8(PIC1_ICW4, 0x01);  //  无缓冲区模式

    writePort8(PIC0_IMR, 0xfb);  //  11111011 PIC1以外全部禁止
    writePort8(PIC1_IMR, 0xff);  //  11111111 禁止所有中断
}

// 键盘的中断
void keyBoardInterrupt(int* esp) {
    struct KeyBoardBuf keyBoardBuf;
    writePort8(PIC0_OCW2, 0x61);
    if (!isKeyBufMax()) keyBufPush(readPort8(0x0060));
}

// 鼠标中断
void mouseInterrupt(int* esp) {}

void inthandler27(int* esp)
//  PIC0中断的不完整策略
//  这个中断在Athlon64X2上通过芯片组提供的便利，只需执行一次
//  这个中断只是接收，不执行任何操作
/* 为什么不处理？
        →  因为这个中断可能是电气噪声引发的、只是处理一些重要的情况。*/
{
    writePort8(PIC0_OCW2, 0x67);  //  通知PIC的IRQ-07（参考7-1）
    return;
}
