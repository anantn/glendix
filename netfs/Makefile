TARGET = plan9_net
OBJS = net.o cs.o
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m += $(TARGET).o
$(TARGET)-objs := $(OBJS)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	
clean:
	-rm -f *.o *.ko .*.cmd .*.flags *.mod.c

