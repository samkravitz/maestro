AS = nasm
CC = gcc
CFLAGS = -std=gnu99 -march=i686 -m32 -fno-stack-protector -fno-pie -ffreestanding -nostdlib -Wall -Wextra $(INCLUDE) -I lib/libc
LDFLAGS = -L lib/libc -l:libc.a
INCLUDE = -I include -I lib/libc
VPATH = src/

# C sources
C = \
	frame.c \
	idt.c \
	init.c \
	intr.c \
	io.c \
	kbd.c \
	kmain.c \
	kmalloc.c \
	kout.c \
	mm.c \
	pit.c \
	tty.c

# asm sources
ASM = \
	boot.s \
	irq.s \
	isr.s \
	pdsw.s \

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
	qemu-system-i386 -m 512M -drive file=maestro.bin,format=raw,index=0,media=disk

.PHONY: clean
clean:
	rm -rf *.o *.bin
	$(MAKE) -C lib clean