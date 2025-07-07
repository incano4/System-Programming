# obj-m += crc.o

# all:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# clean:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

obj-m += crc.o

BUILD_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	# lsmod
	make -C $(BUILD_DIR) M=$(PWD) modules -s
	make install
	# lsmod
	sudo dmesg | tail -n 5
	# make uninstall
	# sudo dmesg | tail -n 5
	make clean

clean:
	make -C $(BUILD_DIR) M=$(PWD) clean

install:
	sudo insmod crc.ko

uninstall:
	sudo rmmod crc