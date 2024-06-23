#od -tx2 -N20 -w8 osmd.bin

############################################################# 修改部分
CROSS_COMPILE = aarch64-none-elf-
LINK_SCRIPT = bsp/rk3568/arch64/rtos.lds


############################################################# 基础信息
TARGET = rtos


############################################################# 编译相关
CC		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

AS_OPTION := -g -march=armv8-a -mtune=cortex-a73 -Wl,--build-id=none -fno-builtin -fno-PIE -Wall  -fno-dwarf2-cfi-asm -mcmodel=large -fomit-frame-pointer -fzero-initialized-in-bss -fdollars-in-identifiers -ffunction-sections -fdata-sections -fno-common -fno-aggressive-loop-optimizations -fno-optimize-strlen -fno-schedule-insns -fno-inline-small-functions -fno-inline-functions-called-once -fno-strict-aliasing -fno-builtin -finline-limit=20 -mstrict-align -mlittle-endian -nostartfiles -mgeneral-regs-only -DENV_EL1
CC_OPTION := -g -c 

# AS_OPTION := -g -march=armv8-a -mtune=cortex-a73 -Wl,--build-id=none -fno-builtin -fno-PIE -Wall  -fno-dwarf2-cfi-asm -mcmodel=large -fomit-frame-pointer -fzero-initialized-in-bss -fdollars-in-identifiers -ffunction-sections -fdata-sections -fno-common -fno-aggressive-loop-optimizations -fno-optimize-strlen -fno-schedule-insns -fno-inline-small-functions -fno-inline-functions-called-once -fno-strict-aliasing -fno-builtin -finline-limit=20 -mstrict-align -mlittle-endian -nostartfiles -mgeneral-regs-only -DENV_EL1
# CC_OPTION := -g -c -march=armv8.2-a+nofp+nosimd -Wl,--build-id=none -fno-builtin -fno-PIE -Wall  -fno-dwarf2-cfi-asm -O0 -mcmodel=large  -fomit-frame-pointer -fzero-initialized-in-bss -fdollars-in-identifiers -ffunction-sections -fdata-sections -fno-common -fno-aggressive-loop-optimizations -fno-optimize-strlen -fno-schedule-insns -fno-inline-small-functions -fno-inline-functions-called-once -fno-strict-aliasing -fno-builtin -finline-limit=20 -mstrict-align -mlittle-endian -specs=nosys.specs -nostartfiles -funwind-tables

LD_OPTION := -nostdlib -EL -d -static --wrap memset
AR_OPTION := -r
############################################################# 头文件路径
INCDIRS := 					\
	app/test/				\

############################################################# 源码路径
SRCDIRS :=    				\
	bsp/rk3568/arch64/ 		\
	app/ 					\
	app/test/ 				\

############################################################# 处理部分
INCLUDE 	:= $(patsubst %, -I %, $(INCDIRS))						# 加入-I
SFILES  	:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))	# 搜索.S文件
CFILES  	:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))	# 搜索.c文件
SFILENDIR	:= $(notdir  $(SFILES))
CFILENDIR	:= $(notdir  $(CFILES))
SOBJS		:= $(patsubst %, obj/%, $(SFILENDIR:.S=.o))				# S目标文件转换
COBJS		:= $(patsubst %, obj/%, $(CFILENDIR:.c=.o))				# C目标文件转换
OBJS		:= $(SOBJS) $(COBJS)									# 最后目标是两者
VPATH		:= $(SRCDIRS)
############################################################# 编译
.PHONY: clean

$(TARGET).bin: $(OBJS)
	@$(LD) -Map $(TARGET).map -T $(LINK_SCRIPT) -o $(TARGET).elf $^
	$(OBJCOPY) -O binary -S $(TARGET).elf $@
	@$(OBJDUMP) -D $(TARGET).elf > $(TARGET).dis

$(SOBJS) : obj/%.o : %.S
	$(CC) -c $(INCLUDE) $(AS_OPTION) -o $@ $<

$(COBJS) : obj/%.o : %.c
	$(CC) -c $(INCLUDE) $(CC_OPTION) -o $@ $<

clean:
	@rm -rf $(TARGET).elf $(TARGET).dis $(TARGET).bin $(TARGET).map $(COBJS) $(SOBJS)
	
	
