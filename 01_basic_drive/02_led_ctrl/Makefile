KERNEL_DIR := /work/system/linux-2.6.22.6
PWD := $(shell pwd)
obj-m := second.o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) modules clean