# 基本条件
KDIR = /home/zw/imx6ull/kernel/zwlinux
ARCH = ARCH=arm CROSS_COMPILE=arm-poky-linux-gnueabi-

# 编译条件
ccflags-y += -std=gnu11 -Wno-declaration-after-statement

# 头文件
# ccflags-y += -I$(PWD)/../common

# 目标文件
obj-m := xxx.o

all:
	$(MAKE) $(ARCH) -C $(KDIR) M=$(PWD) modules
	@rm -rf *.o .*.o.cmd .*.ko.cmd *.order *.mod.c *.mod.o *.symvers .tmp_versions

clean:
	$(MAKE) $(ARCH) -C $(KDIR) M=$(PWD) clean

