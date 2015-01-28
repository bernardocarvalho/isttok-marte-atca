TARGET = TriggerTestMod

EXTRA_CFLAGS = -DEXPORT_SYMTAB -I/usr/realtime/include -D_RTAI
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
KPWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/kernel/$(MDIR)

obj-m	:= $(TARGET).o

$(TARGET)-objs := TriggerTest.o

default:
	make -C $(KDIR) SUBDIRS=$(KPWD) modules
clean:
	-rm -f *.o *.a *.ko .*.cmd .*.flags *.mod.c *.s
