CROSS_COMPILE=arm-poky-linux-gnueabi

all:
	$(CROSS_COMPILE)-gcc -g -c start.S -o start.o
	$(CROSS_COMPILE)-gcc -g -c main.c -o main.o
	$(CROSS_COMPILE)-ld -Tlink.lds start.o main.o -o main.elf
	$(CROSS_COMPILE)-objcopy -O binary -S -g main.elf main
	$(CROSS_COMPILE)-objdump -D main.elf > main.dis

	mkdir -p _build
	mv start.o main.o main.elf _build

clean:
	rm main main.dis _build -rf 
