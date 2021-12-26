TOOLPATH= ../../tools/
MAKE = make
NASK = $(TOOLPATH)nask.exe
EDIMG = $(TOOLPATH)edimg.exe

IPL.img: IPL.asm
	$(NASK) ./IPL.asm ./IPL.img ./ILP.list

MyOS.sys: MyOS.asm
	$(NASK) ./MyOS.asm ./MyOS.sys

MyOS.img: MyOS.sys IPL.img
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:IPL.img len:512 from:0 to:0 \
		copy from:MyOS.sys to:@: \
		imgout:MyOS.img

install:
	$(MAKE) MyOS.img



