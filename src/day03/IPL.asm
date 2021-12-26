; day03 制作IPL 读取磁盘启动扇区到内存
; MyOS
; TAB = 4

CYLS    EQU     10         ; 定义 CYLS = 10
    ORG     0x7c00         ; 程序装载地址

; 标准FAT12格式软盘专用代码

    JMP     entry
    DB      0x90
    DB      "MyOS    "               ; 启动扇区名称 必须是8字节
    DW      512                      ; 扇区大小 必须为512字节
    DB      1                        ; 簇大小 必须为一个扇区
    DW      1                        ; FAT启示位置 一般为第一个扇区
    DB      2                        ; FAT个数 必须是2
    DW      224                      ; 根目录大小 一般是224
    DW      2880                     ; 磁盘大小 必须为2280扇区
    DB      0xf0                     ; 磁盘类型  必须是0xf0
    DW      9                        ; FAT长度   必须是9个扇区
    DW      18                       ; 一个磁道扇区数 必须是18
    DW      2                        ; 磁头数    必须是2
    DD      0                        ; 不使用分区 必须是0
    DD      2880                     ; 磁盘大小 重写了一次
    DB      0,0,0x29                 ; 意义不明 写法固定
    DD      0xffffffff		         ; 可能是卷标号码
    DB      "MyOS       "            ; 磁盘名称 必须是11字节
    DB      "FAT12   "               ; 磁盘格式名称 必须是8字节
    RESB    18                       ; 空出18字节

; 程序主体

entry:
    MOV     AX, 0                    ; 初始化寄存器   
    MOV     SS, AX
    MOV     SP, 0x7c00
    MOV     DS, AX

; 读取磁盘 2 - 18 扇区(512 * (18 - 2 + 1)字节)

    MOV     AX, 0x0820               ; 读取的内容放到 0x08200
    MOV     ES, AX                   ; ES 才是真正存放的位置
    MOV     CH, 0                    ; 柱面0
    MOV     DH, 0                    ; 磁头0
    MOV     CL, 2                    ; 读取的扇区号

readLoop:
    MOV     SI, 0                    ; 记录读取失败的次数

retry:                               ; 准备调用读盘操作
    MOV     AH, 0x02                 ; AH = 0x02 为读盘操作
    MOV     AL, 1                    ; 要读取的磁盘数
    MOV     BX, 0                    
    MOV     DL, 0x00                 ; A驱动器
    INT     0x13                     ; 调用读取操作
    JNC     next                     ; 没有出错读取下一个磁盘
    ADD     SI, 1                    ; 出错了
    CMP     SI, 5                    
    JAE     error                    ; SI >= 5
    MOV     AH, 0x00
    MOV     DL, 0x00
    INT     0x13                     ; 重置驱动器
    JMP     retry                   

next:                                ; 准备读取下一个扇区, 这里必须要用循环不能直接把AL直接设为 16
    MOV     AX, ES
    ADD     AX, 0x0020                 ; 内存位置向后移0x20(512的16进制, 1个扇区的大小)
    MOV     ES, AX
    ADD     CL, 1
    CMP     CL, 18
    JBE     readLoop                 ; 循环读取到扇区18            
    MOV     CL, 1
    ADD     DH, 1                    ; 磁头1 软盘的反面
    CMP     DH, 2                    ; DH = 2 时 没有磁头3
    JB      readLoop                 ; 循环读取磁盘的正反两面
    MOV     DH, 0
    ADD     CH, 1
    CMP     CH, CYLS
    JB      readLoop                 ; 循环读取柱面到 柱面10
    
; 读取完毕, 跳转到haribote.sys执行
    MOV     [0x0ff0], CH
    JMP     0xc200
    
error:
    MOV     SI, msg

showImgLoop:
    MOV     AL, [SI]
    ADD     SI, 1
    CMP     AL, 0
    JE      finishShow
    MOV     AH, 0x0e
    MOV     BX, 15
    INT     0x10
    JMP     showImgLoop

finishShow:
    HLT
    JMP     finishShow
msg:
    DB      0x0a, 0x0a
    DB      "Load address error"
    DB      0x0a
    DB      0
    RESB    0x7dfe-$                ; 填写 0 到0x001fe

    DB      0x55, 0xaa              ; 启动扇区的结束标志 