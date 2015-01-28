TARGET = pcieAdcMod

EXTRA_CFLAGS = -DEXPORT_SYMTAB 
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
KPWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/kernel/$(MDIR)

obj-m	:= $(TARGET).o

$(TARGET)-objs := pcieAdc.o pcieAdc_proc.o

default:
	make -C $(KDIR) SUBDIRS=$(KPWD) modules
clean:
	-rm -f *.o *.a *.ko .*.cmd .*.flags *.mod.c *.s
