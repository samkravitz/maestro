AS = nasm
CC = gcc
CFLAGS = -std=gnu99 -march=i686 -m32 -fno-stack-protector -fno-pie -ffreestanding -nostdlib -Wall -Wextra $(INCLUDE)
LDFLAGS = -L lib/libc -l:libc.a -L lib/libdlmalloc -l:libdlmalloc.a
INCLUDE = -I include -I lib/libc -I lib/libdlmalloc
VPATH = src/

# C sources
C = \
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
	irq.s \
	isr.s \
	pdsw.s \

OBJ = $(C:.c=.o) $(ASM:.s=.o)

all: libs maestro.bin iso

maestro.bin: $(OBJ) $(LIBS)
	$(CC) $(CFLAGS) -T linker.ld -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) -f elf32 $< -o $@

libs:
	$(MAKE) -C lib

iso: maestro.bin
	mkdir -p iso/boot/grub
	cp maestro.bin iso/boot/maestro.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o maestro.iso iso

.PHONY: test
test:
	$(MAKE) -C test

# # qemu-system-i386 -m 512M -drive file=maestro.bin,format=raw,index=0,media=disk
.PHONY: start
start:
	qemu-system-i386 -cdrom maestro.iso
	

.PHONY: clean
clean:
	rm -rf *.o *.bin *.iso
	rm -rf iso
	$(MAKE) -C lib clean
	$(MAKE) -C test clean