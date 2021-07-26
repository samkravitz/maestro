AS = nasm
CC = ${HOME}/opt/cross/bin/i686-elf-gcc

C_SOURCES = main.c
ASM_SOURCES = boot.s

OBJ = ${C_SOURCES:.c=.o} ${ASM_SOURCES:.s=.o}

all: kernel.bin

%.o: %.c
	${CC} -c $< -o $@ -std=c99 -ffreestanding -O2 -Wall -Wextra

%.o: %.s
	${AS} -f elf32 $< -o $@

kernel.bin: ${OBJ}
	${CC} -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $^ -lgcc

.PHONY: start
start:
	qemu-system-i386 -fda kernel.bin


.PHONY: clean
clean:
	rm -rf *.o *.bin