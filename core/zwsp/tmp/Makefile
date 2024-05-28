# 基本条件
KDIR = /home/zw/hz/edge/kernel/linux-5.10
ARCH = ARCH=arm64 CROSS_COMPILE=/home/zw/hz/edge/prebuilts/gcc/linux-x86/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-
CC   = /home/zw/hz/edge/prebuilts/clang/bin/clang

# 编译条件
ccflags-y += -std=gnu11 -Wno-declaration-after-statement

# 头文件
#ccflags-y += -I$(PWD)/../common

# 目标文件
obj-m := xxx.o

all:
	$(MAKE) $(ARCH) CC=$(CC) LLVM=1 LLVM_IAS=1 -C $(KDIR) M=$(PWD)
	@rm -rf *.o .*.o.cmd .*.ko.cmd *.order *.mod.c *.mod.o *.symvers .tmp_versions *.mod

clean:
	$(MAKE) $(ARCH) -C $(KDIR) M=$(PWD) clean

