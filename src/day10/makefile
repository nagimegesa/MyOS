TOOLPATH = ../../tools/
INCPATH =  $(TOOLPATH)MyOSHead/

MAKE = make -r
NASK = $(TOOLPATH)nask.exe
EDIMG = $(TOOLPATH)edimg.exe
CC1 = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet -std=c99
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM = $(TOOLPATH)obj2bim.exe
RULEFILE = $(TOOLPATH)MyOSHead/MyOS.rul
BIM2HRB = $(TOOLPATH)bim2hrb.exe
MAKEFONT = $(TOOLPATH)makefont.exe
BIN2OBJ  = $(TOOLPATH)bin2obj.exe

TRAGET = BaseFunction.obj graphic.obj dsctbl.obj \
	bootpack.obj interrupt.obj buf.obj KeyBoard.obj \
	mouse.obj memory.obj 

defualt:
	$(MAKE) MyOS.img

BaseFunction.obj: BaseFunction.s
	$(NASK) BaseFunction.s BaseFunction.obj

%.gas: %.c
	$(CC1) $*.c -o $*.gas

%.nas: %.gas
	$(GAS2NASK) $*.gas $*.nas

%.obj: %.nas 
	$(NASK) $*.nas $*.obj

bootpack.bim:  $(TRAGET)
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(TRAGET)

bootpack.hrb: bootpack.bim
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

MyOS.sys: bootpack.hrb MyOS.bin
	copy /B MyOS.bin+bootpack.hrb MyOS.sys

IPL.sys: IPL.s
	$(NASK) ./IPL.s ./IPL.sys ./ILP.list

MyOS.bin: MyOS.s
	$(NASK) ./MyOS.s ./MyOS.bin

MyOS.img: MyOS.sys IPL.sys
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:IPL.sys len:512 from:0 to:0 \
		copy from:MyOS.sys to:@: \
		imgout:MyOS.img

clean:
	del *.gas
	del *.obj
	del *.sys
	del *.bin
	del *.list
	del *.nas
	del *.bim
	del *.hrb
	del *.map

cleanAll:
	$(MAKE) clean
	del *.img