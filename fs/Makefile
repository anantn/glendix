# We have been called from the kernel build system
ifneq ($(KERNELRELEASE),)

# Ugh, if you're building it alone, you don't have CONFIG_BINFMT_PLAN9
# anywhere...
ifneq ($(GLENDIX_STANDALONE),)
	obj-m += binfmt_plan9.o binfmt_plan9_syscalls.o
else
	obj-$(CONFIG_BINFMT_PLAN9) += binfmt_plan9.o binfmt_plan9_syscalls.o
endif

# Not a kernel build, make the modules by hand
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) GLENDIX_STANDALONE=1 M=$(PWD) modules
endif
