#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

// 初始化PIC
void initPic(void);

// 键盘中断
void keyBoardInterrupt(int* esp);

// 鼠标中断
void mouseInterrupt(int* esp);

// 其他的中断
void inthandler27(int* esp);

/**
 *  1. IMR 是中断屏蔽寄存器(8)位，为1 的那一位对应的PIC被屏蔽
 *  2. ICW 是初始化数据寄存器
 */
#define PIC0_ICW1 0x0020
#define PIC0_OCW2 0x0020
#define PIC0_IMR 0x0021
#define PIC0_ICW2 0x0021
#define PIC0_ICW3 0x0021
#define PIC0_ICW4 0x0021
#define PIC1_ICW1 0x00a0
#define PIC1_OCW2 0x00a0
#define PIC1_IMR 0x00a1
#define PIC1_ICW2 0x00a1
#define PIC1_ICW3 0x00a1
#define PIC1_ICW4 0x00a1

#endif