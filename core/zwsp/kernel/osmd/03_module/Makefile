# 基本条件
KDIR = /home/zw/01_project/01_mica/01_edge/01_edge/kernel/linux-5.10
ARCH = ARCH=arm64 CROSS_COMPILE=/home/zw/01_project/01_mica/01_edge/01_edge/prebuilts/gcc/linux-x86/aarch64/aarch64/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-
CC   = /home/zw/hz/01_micad/edge/prebuilts/clang/bin/clang

# 编译条件
ccflags-y += -std=gnu11 -Wno-declaration-after-statement -D__KERNEL_SYSCALLS__

# 头文件
#ccflags-y += -I$(PWD)/attr_group/xxkernel

# 目标文件
obj-m := osmd.o


all:
	$(MAKE) $(ARCH) CC=$(CC) LLVM=1 LLVM_IAS=1 -C $(KDIR) M=$(PWD)
	@rm -rf *.o .*.o.cmd .*.ko.cmd *.order *.mod.c *.mod.o *.symvers .tmp_versions *.mod 
	@chmod +x osmd.ko

clean:
	$(MAKE) $(ARCH) -C $(KDIR) M=$(PWD) clean

