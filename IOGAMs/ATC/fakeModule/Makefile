TARGET = pcieAdcMod

EXTRA_CFLAGS = -DEXPORT_SYMTAB -I/usr/realtime/include -D_RTAI -mhard-float
CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
KPWD = $(shell pwd)
DEST = /lib/modules/$(CURRENT)/kernel/$(MDIR)

$(LDFLAGS_NOKVERSION) = -T $(PWD)/../../../../BaseLib2/LevelOS/rtai/C++Sup/global_obj_link.ld

obj-m	:= $(TARGET).o

$(TARGET)-objs := pcieAdc.o

default:
	make -C $(KDIR) SUBDIRS=$(KPWD) modules
clean:
	-rm -f *.o *.a *.ko .*.cmd .*.flags *.mod.c *.s
