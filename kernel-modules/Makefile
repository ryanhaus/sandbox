ifneq ($(KERNELRELEASE),)
	obj-m := hello.o char_test.o addition.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
