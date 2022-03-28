include config.mk

VPATH = src/ src/bootloader

# C sources
C = \
	ata.c \
	clk.c \
	ext2.c \
	idt.c \
	init.c \
	io.c \
	isr.c \
	kbd.c \
	kmain.c \
	kmalloc.c \
	kprintf.c \
	pmm.c \
	pq.c \
	proc.c \
	sched.c \
	tty.c \
	vfs.c \
	vmm.c 

# asm sources
ASM = \
	ctxsw.s \
	disable.s \
	intr.s \
	start.s

USER = \
	user/ls/ls.o

OBJ = $(addprefix bin/, $(C:.c=.o) $(ASM:.s=.o))

all: libs maestro.bin bootloader img user

maestro.bin: $(OBJ) $(USER)
	$(LD) -o $@ $^ $(LDFLAGS)

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

user:
	$(MAKE) -C user

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
	qemu-system-i386 \
		-m 16M \
		-serial stdio \
		-drive file=disk.img,format=raw,index=0,media=disk

.PHONY: clean
clean:
	rm -f bin/* *.bin
	$(MAKE) -C lib clean
	$(MAKE) -C user clean
	$(MAKE) -C test clean
