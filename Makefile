CC = gcc
AS = nasm
CFLAGS = -std=gnu99 -march=i686 -m32 -fno-stack-protector -no-pie -ffreestanding -nostdlib -Wall -Wextra $(INCLUDE)
LDFLAGS =
INCLUDE = -I include -I lib/libc -I lib/libdlmalloc
VPATH = src/ lib/libc lib/libdlmalloc

# C sources
C = \
	ata.c \
	clk.c \
	dump.c \
	idt.c \
	init.c \
	intr.c \
	io.c \
	kbd.c \
	kmain.c \
	kmalloc.c \
	kout.c \
	mm.c \
	pq.c \
	proc.c \
	sched.c \
	tty.c

# asm sources
ASM = \
	boot.s \
	ctxsw.s \
	disable.s \
	irq.s \
	isr.s \
	pdsw.s \

# lib sources
LIB = \
	stdlib.c \
	string.c \
	malloc.c \

OBJ = $(C:.c=.o) $(ASM:.s=.o) $(LIB:.c=.o)

all: libs maestro.bin iso

maestro.bin: $(OBJ)
	$(CC) $(CFLAGS) -T linker.ld -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) -f elf32 $< -o $@

libs:
	$(MAKE) -C lib

iso: maestro.bin
	mkdir -p iso/boot/grub
	cp maestro.bin iso/boot
	cp grub.cfg iso/boot/grub
	grub-mkrescue -o maestro.iso iso

.PHONY: test
test:
	$(MAKE) -C test

.PHONY: start
start:
	qemu-system-i386 -hda disk.img -cdrom maestro.iso
	

.PHONY: clean
clean:
	rm -rf *.o *.bin *.iso
	rm -rf iso
	$(MAKE) -C lib clean
	$(MAKE) -C test clean

# creates a 1M raw disk image for maestro
.PHONY: disk
disk:
	qemu-img create -f raw disk.img 1M