#od -tx2 -N20 -w8 osmd.bin

TARGET=osmd
LOAD_ADDR=0x70000000
CROSS_COMPILE=aarch64-openeuler-linux-gnu-

all:
	@$(CROSS_COMPILE)gcc -g -c start.S -o start.o
	@$(CROSS_COMPILE)ld -Ttext $(LOAD_ADDR) start.o -o $(TARGET).elf
	@$(CROSS_COMPILE)objcopy -O binary -S -g $(TARGET).elf $(TARGET).bin
	@$(CROSS_COMPILE)objdump -D $(TARGET).elf > $(TARGET).dis
	@chmod +x *.dis
	@rm *.o

clean:
	@rm -rf *.o *.bin *.elf *.dis
	
	
