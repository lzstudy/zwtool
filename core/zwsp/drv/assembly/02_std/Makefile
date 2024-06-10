CROSS_COMPILE=arm-poky-linux-gnueabi

all:
	$(CROSS_COMPILE)-gcc -g -c start.S -o start.o
	$(CROSS_COMPILE)-ld -Ttext 0x80800000 start.o -o main.elf
	$(CROSS_COMPILE)-objcopy -O binary -S -g main.elf main
	$(CROSS_COMPILE)-objdump -D main.elf > main.dis

	mkdir -p _build
	mv main.elf start.o _build

clean:
	rm main main.dis _build -rf 
