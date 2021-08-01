AS = nasm
CC = gcc
CFLAGS = -std=c99 -march=i386 -m32 -fno-stack-protector -fno-pie -ffreestanding -nostdlib -O2 -Wall -Wextra -I $(INCLUDE) -I lib/libc
LDFLAGS = -L lib/libc -l:libc.a
INCLUDE = include/
VPATH = src/

# C sources
C = \
	gdt.c \
	idt.c \
	inittab.c \
	io.c \
	klog.c \
	kmain.c \
	txtmode.c

# asm sources
ASM = \
	boot.s \
	isr.s \

OBJ = $(C:.c=.o) $(ASM:.s=.o)

all: libs maestro.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) -f elf32 $< -o $@

maestro.bin: $(OBJ) $(LIBS)
	$(CC) $(CFLAGS) -T linker.ld -o $@ $^ $(LDFLAGS)

libs:
	$(MAKE) -C lib

.PHONY: start
start:
	qemu-system-i386 -drive file=maestro.bin,format=raw,index=0,media=disk


.PHONY: clean
clean:
	rm -rf *.o *.bin
	$(MAKE) -C lib clean