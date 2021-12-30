TOOLPATH= ../../tools/
MAKE = make -r
NASK = $(TOOLPATH)nask.exe
EDIMG = $(TOOLPATH)edimg.exe
CC1 = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM = $(TOOLPATH)obj2bim.exe
RULEFILE = $(TOOLPATH)MyOSHead/MyOS.rul
BIM2HRB = $(TOOLPATH)bim2hrb.exe

IPL.sys: IPL.asm
	$(NASK) ./IPL.asm ./IPL.sys ./ILP.list

MyOS.bin: MyOS.asm
	$(NASK) ./MyOS.asm ./MyOS.bin

MyOS.img: MyOS.sys IPL.sys
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:IPL.sys len:512 from:0 to:0 \
		copy from:MyOS.sys to:@: \
		imgout:MyOS.img

bootpack.gas: bootpack.c
	$(CC1)	bootpack.c -o bootpack.gas 

bootpack.nas: bootpack.gas
	$(GAS2NASK) bootpack.gas bootpack.nas

bootpack.obj: bootpack.nas
	$(NASK) bootpack.nas bootpack.obj

MyFunction.obj: MyFunction.asm
	$(NASK) MyFunction.asm MyFunction.obj

bootpack.bim: bootpack.obj MyFunction.obj
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		bootpack.obj MyFunction.obj

bootpack.hrb: bootpack.bim
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

MyOS.sys: bootpack.hrb MyOS.bin
	copy /B MyOS.bin+bootpack.hrb MyOS.sys

install:
	$(MAKE) MyOS.img

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