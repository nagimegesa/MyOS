; MyFunction

[FORMAT "WCOFF"]              ; 产生文件的模式
[BITS 32]                     ; 制作32位的汇编语言

[FILE "MyFunction.asm"]       ; 文件名
    GLOBAL  _hlt              ; 全局函数的声明

[SECTION .text]

_hlt:              ; void hlt(void)    
    HLT
    RET
