CC = gcc
CXX = g++
AS = nasm
CFLAGS = -std=gnu99 -march=i686 -m32 -fno-stack-protector -no-pie -ffreestanding -nostdlib -Wall -Wextra $(INCLUDE)
CXXFLAGS = -std=c++17 -m32 -fno-pie -fno-stack-protector -fno-exceptions -fno-rtti -ffreestanding -nostdlib -O2 -Wall -Wextra $(INCLUDE)
LDFLAGS = -L lib/libc -l:libc.a -L lib/mxx -l:mxx.a
INCLUDE = -I include -I lib/libc -I lib/mxx
VPATH = src/ lib/libc

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

# C++ sources
CPP = \
	plus.cpp \

# asm sources
ASM = \
	boot.s \
	ctxsw.s \
	disable.s \
	irq.s \
	isr.s \
	pdsw.s \

OBJ = $(addprefix bin/, $(C:.c=.o) $(CPP:.cpp=.o) $(ASM:.s=.o))

all: libs maestro.bin img

maestro.bin: $(OBJ)
	$(CC) $(CFLAGS) -T linker.ld -o $@ $^ $(LDFLAGS)

bin/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
bin/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/%.o: %.s
	$(AS) -f elf32 $< -o $@

libs:
	$(MAKE) -C lib

img: maestro.bin
	mkdir mnt
	sudo mount -t ext2 -o loop,offset=1048576 disk.img mnt
	sudo cp maestro.bin mnt/boot
	sudo cp grub.cfg mnt/boot/grub
	sync
	sudo umount mnt
	rmdir mnt

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
