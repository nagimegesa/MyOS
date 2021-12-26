; MyOS
; TAB = 4
    ORG     0x7c00         ; 程序装载地址

; 标准FAT12格式软盘专用代码

    JMP     entry
    DB      0x90
    DB      "MyOS    "               ; 启动扇区名称 必须是8字节
    DW      512                      ; 扇区大小 必须为512字节
    DB      1                        ; 簇大小 必须为一个扇区
    DW      1                        ; FAT启示位置 一般为第一个扇区
    DB      1                        ; FAT个数 必须是2
    DW      224                      ; 根目录大小 一般是224
    DW      2880                     ; 磁盘大小 必须为2280扇区
    DB      0xf0                     ; 磁盘类型  必须是0xf0
    DW      9                        ; FAT长度   必须是9个扇区
    DW      18                       ; 一个磁道扇区数 必须是18
    DW      2                        ; 磁头数    必须是2
    DD      0                        ; 不使用分区 必须是0
    DD      2880                     ; 磁盘大小 重写了一次
    DB      0,0,0x29                 ; 意义不明 写法固定
    DD      0xffffffff		        ; 可能是卷标号码
    DB      "MyOS       "            ; 磁盘名称 必须是11字节
    DB      "FAT12   "               ; 磁盘格式名称 必须是8字节
    RESB    18                       ; 空出18字节

; 程序主体

entry:                               ; 初始化寄存器
    MOV     AX,0                     ; AX = 0
    MOV     SS,AX                    ; SS=AX(0)
    MOV     SP,0x7c00                ; SP(栈指针) = 0x7c00
    MOV     DS,AX                    ; DS = AX(0)
    MOV     ES,AX

    MOV     SI,msg

putloop:
    MOV     AL,[SI]
    ADD     SI,1
    CMP     AL,0
    JE      fin
    MOV     AH,0x0e                 ; 显示一个文字
    MOV     BX,15                   ; 指定字符颜色
    INT     0x10                    ; 调用显卡的中断
    JMP     putloop

fin:
    HLT                             ; cpu休眠
    JMP     fin

msg:
    DB      0x0a,0x0a               ; 换行两次
    DB      "hello zzq, I am your os"
    DB      0x0a
    DB      0

    RESB    0x7dfe-$

    DB      0x55,0xaa