ifneq ($(KERNELRELEASE),)
	obj-m += net.o cs.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) GLENDIX_STANDALONE=1 M=$(PWD) modules
endif
