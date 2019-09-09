KERNDIR :=/lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
obj-m := first_1.o

all:
	make -C $(KERNDIR) M=$(PWD) modules
clean:
	make -C $(KERNDIR) M=$(PWD) modules clean


