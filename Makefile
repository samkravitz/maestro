CC = gcc
CXX = g++
AS = nasm
CFLAGS = -std=gnu99 -march=i686 -m32 -fno-stack-protector -fno-pie -ffreestanding -nostdlib -Wall -Wextra $(INCLUDE)
CXXFLAGS = -std=c++17 -m32 -fno-pie -fno-stack-protector -fno-exceptions -fno-rtti -ffreestanding -nostdlib -O2 -Wall -Wextra $(INCLUDE)
LDFLAGS = -L lib/libc -l:libc.a
INCLUDE = -I include -I lib/libc -I lib/mxx
VPATH = src/ src/bootloader lib/libc

# C sources
C = \
	ata.c \
	clk.c \
	ext2.c \
	fs.c \
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
	ctxsw.s \
	disable.s \
	irq.s \
	isr.s \
	pdsw.s \
	start.s

OBJ = $(addprefix bin/, $(C:.c=.o) $(ASM:.s=.o))

all: libs maestro.bin bootloader img 

maestro.bin: $(OBJ)
	$(CC) $(CFLAGS) -T linker.ld -o $@ $^ $(LDFLAGS)

stage1.bin: stage1.s
	$(AS) -i src/bootloader -f bin $< -o $@
	dd if=stage1.bin of=disk.img conv=notrunc

stage2.bin: stage2.s
	$(AS) -i src/bootloader -f bin $< -o $@
	e2cp stage2.bin disk.img:/

bin/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
bin/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/%.o: %.s
	$(AS) -f elf32 $< -o $@

libs:
	$(MAKE) -C lib

disk:
	meta/make_disk.sh

bootloader: stage1.bin stage2.bin

img: maestro.bin
	e2cp maestro.bin disk.img:/

.PHONY: test
test:
	$(MAKE) -C test

.PHONY: start
start:
	qemu-system-i386 -m 16M -drive file=disk.img,format=raw,index=0,media=disk

.PHONY: clean
clean:
	rm -f bin/* *.bin
	rm -rf mnt
	$(MAKE) -C lib clean
	$(MAKE) -C test clean
