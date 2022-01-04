; MyFunction

[FORMAT "WCOFF"]              ; 产生文件的模式
[INSTRSET "i486p"]
[BITS 32]                     ; 制作32位的汇编语言
[FILE "MyFunction.asm"]       ; 文件名
    GLOBAL  _pause              ; 全局函数的声明
    GLOBAL  _writeToMemory
    GLOBAL  _setLock, _closeLock, _closeLockAndPause
    GLOBAL  _readPort8,  _readPort16,  _readPort32       ; 获得端口的信息
    GLOBAL  _writePort8, _writePort16, _writePort32      ; 向端口发出信息
    GLOBAL  _getLockTag, _setLockTag            ; 锁函数
    GLOBAL	_loadGdtr, _loadIdtr
	GLOBAL	_asmKeyBoardInterrupt, _asm_inthandler27, _asmMouseInterrupt
    EXTERN  _keyBoardInterrupt, _mouseInterrupt, _inthandler27

[SECTION .text]

_pause:              ; void hlt(void)    
    HLT
    RET

_writeToMemory:    ; writeToMemoey(int addr, int data);
    MOV     ECX, [ESP + 4]  ; [ESP + 4] 存放的地址
    MOV     AL, [ESP + 8]   ; [ESP + 8] 存放的data
    MOV     [ECX], AL       ; 在addr处写入 data
    RET                     ; 返回


_setLock:	; void io_cli(void);   禁止中断
		CLI
		RET

_closeLock:	; void io_sti(void);  开启中断
		STI
		RET

_closeLockAndPause:	; void io_stihlt(void);    开启中断后休眠
		STI
		HLT
		RET

_readPort8:	; int io_in8(int port);    读8位端口
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AL,DX
		RET

_readPort16:	; int io_in16(int port); 读16位端口
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,0
		IN		AX,DX
		RET

_readPort32:	; int io_in32(int port); 读32位端口
		MOV		EDX,[ESP+4]		; port
		IN		EAX,DX
		RET

_writePort8:	                ; void io_out8(int port, int data); 写8位端口
		MOV		EDX,[ESP+4]		; port
		MOV		AL,[ESP+8]		; data
		OUT		DX,AL
		RET

_writePort16:	                ; void io_out16(int port, int data); 写16位端口
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,AX
		RET

_writePort32:	                ; void io_out32(int port, int data); 写32位端口
		MOV		EDX,[ESP+4]		; port
		MOV		EAX,[ESP+8]		; data
		OUT		DX,EAX
		RET

_getLockTag:	            ; int io_load_eflags(void); 获得中断状态
		PUSHFD		        ; PUSH EFLAGS 
		POP		EAX
		RET

_setLockTag:	            ; void io_store_eflags(int eflags); 设置中断状态
		MOV		EAX,[ESP+4]
		PUSH	EAX
		POPFD	            ; POP EFLAGS 
		RET
_loadGdtr:		; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_loadIdtr:		; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_asmKeyBoardInterrupt:      ; 键盘中断的准备, 储存相关的信息 
    PUSH        ES 
    PUSH        DS 
    PUSHAD 
    MOV         EAX, ESP 
    PUSH        EAX 
    MOV         AX, SS 
    MOV         DS, AX 
    MOV         ES, AX 
    CALL        _keyBoardInterrupt 
    POP         EAX 
    POPAD 
    POP         DS 
    POP         ES 
    IRETD

_asmMouseInterrupt:     ;鼠标中断的准备, 储存相关的信息 
    PUSH ES 
    PUSH DS 
    PUSHAD 
    MOV EAX,ESP 
    PUSH EAX 
    MOV AX, SS 
    MOV DS, AX 
    MOV ES,AX 
    CALL _mouseInterrupt 
    POP EAX 
    POPAD 
    POP DS 
    POP ES 
    IRETD

_asm_inthandler27: 
    PUSH ES 
    PUSH DS
    PUSHAD 
    MOV EAX, ESP 
    PUSH EAX 
    MOV AX,SS 
    MOV DS, AX 
    MOV ES, AX 
    CALL _inthandler27 
    POP EAX 
    POPAD 
    POP DS 
    POP ES 
    IRETD