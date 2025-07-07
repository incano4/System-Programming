# obj-m += hide_proc.o

# all:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# clean:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

obj-m += hide_proc.o

BUILD_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	# lsmod
	make -C $(BUILD_DIR) M=$(PWD) modules -s
	# make install
	# lsmod
	# sudo dmesg | tail -n 5
	# make uninstall
	# sudo dmesg | tail -n 5
	# make clean

clean:
	make -C $(BUILD_DIR) M=$(PWD) clean

install:
	sudo insmod hide_proc.ko

uninstall:
	sudo rmmod hide_proc



# 	obj-m += hideproc.o

# all:
#   make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# clean:
#   make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
