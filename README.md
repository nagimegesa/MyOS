- [文件结构及相关说明](#文件结构及相关说明)
- [Day01](#day01)
- [Day02](#day02)
- [Day03](#day03)

## 文件结构及相关说明
1. src      -- 源码
2. img     -- 资料图片
3. tools    -- 使用的工具
   1. bz1621   -- 十六进制编辑器(二进制编辑器)
   2. MyOSHead -- 自己的操作系统需要的头文件(由作者提供，里面记录了主函数入口的等相关信息)
   3. nask.exe -- 汇编转镜像的程序
   4. eding.exe    -- 合并两个映像文件的工具
   5. fdimd0at.tek -- edimg.exe 的辅助程序，代表空白软盘
   6. cc1.exe     -- c语言转汇编的程序，由gcc改造而来
   7. gas2nask.exe --将gas汇编转化为nask汇编，以便自己的操作系统可以读取
   8. obj2bim  --链接器, 把nask.exe 产生的未链接的文件链接起来
## Day01
1. 使用汇编编写了磁盘的格式
2. 汇编命令
   1.  **DB**:<br> “define byte”的缩写，也就是往文件里直接写入1个字节的指令
   2.  **DW**:<br> "define world"的缩写，也就是往文件里直接写入2个字节的指令
   3.  **DD**:<br> "define double-world"的缩写，也就是往文件里直接写入4个字节的指令
   4.  RESB:<br> 预留空间
3. 寄存器
   1. <br><center>8位寄存器</center>
   ![avatar](./img/8位寄存器.png)
   1. <br><center>16位寄存器</center>
   ![avatar](./img/16位寄存器.png)
   1. <br><center>段位寄存器</center>
   ![avatar](./img/段寄存器.png)
## Day02
1. 使用汇编输出了hello zzq, I am your OS. (*^_^*)
## Day03
1.  制作IPL(**启动程序加载器**也叫**引导加载程序**)加载启动扇区到内存(起始位置0x8200)
    1.  BIOS在启动时会自动读取第一个扇区到内存，这个扇区中记录了磁盘和操作系统相关的信息，并由此引导系统进入操作系统，也因此IPL.asm对应的是位于软盘正面的第一个扇区(512字节)的程序, 即C0-H0-S1上的程序
    2.  1张软盘有80个柱面，2个磁头，18个扇区，且一个扇区有512字节。所以，一张软盘的容量是：80×2×18×512 = 1 474 560 Byte = 1 440KB
    3.  IPL.asm读取了正反两面磁盘的2-18扇区(反面是0-18)的0-9柱面(总共 2 * 10 * 18 * 512 = 184320 字节)到0x80000处
    4.  内存中程序的分布情况见下图
   ![pic](./img/内存分布.png)<br><center>内存分布图</center>

2. 正式开始制作操作系统
   1. 编写MyOS.asm 为操作系统源码, 生成MyOS.sys文件
   2. MyOS.asm对应的是IPL.asm要读取的内容, FAT12格式中0x00260后开始放文件名，0x00420后开始放文件对应的具体内容, 所以MyOS.asm对应的程序被读取到了内存的0x8000+0x4200=0xc200处
   3. 使用<a href = "./tools/edimg.exe">edimg.exe</a>(用法见makefile)合并MyOS.sys和IPL.img 生成MyOS.img
   4. MyOS.img 就是最终的整个软盘

3. 导入C语言
   1. 编写bootpack.c文件(里面放置操作系统的入口函数**OSMain**)
   2. 使用cc1.exe编译后，使用gas2nas转化成nas汇编
      1. cc1.exe是作者提供的C语言编译器,据说由gcc改造而成，产生gas汇编
   3. 用naks.exe 产生相应的映像文件(未链接)，然后使用obj2bim.exe以及bim2hrb产生链接文件
   4. 最后使用windows cmd自带的 **copy /B** 命令合并产生的链接文件与原本操作系统对应的汇编文件(注意使用copy /B的时候要把操作系统的汇编文件**放在加号的前面**)
4. 使用汇编编写C语言函数
   1. 使用汇编编写的函数函数名前要加一个下划线(比如C语言声明void hlt(void) 则在汇编文件要声明为 **_hlt** )<center>使用如下模板编写汇编函数<center>
   ![image-20211226180606958](.\img\汇编编写C函数模板.png)<br>
   2. 写好汇编文件后使用 naks.exe 生成obj 文件在obj2bim步加入一起链接即可