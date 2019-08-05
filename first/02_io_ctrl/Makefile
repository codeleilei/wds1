KERNDIR :=/work/system/linux-2.6.22.6
PWD := $(shell pwd)
obj-m := first_1.o

all:
	make -C $(KERNDIR) M=$(PWD) modules
clean:
	make -C $(KERNDIR) M=$(PWD) modules clean


