obj-m := listen_stat.o
modules-objs:= listen_stat.o

KDIR := /lib/modules/`uname -r`/build
PWD := $(shell pwd)

default:
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -rf *.o .cmd *.ko *.mod.c .tmp_versions *.unsigned *.order *.symvers .listen_stat*

