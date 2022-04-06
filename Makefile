include config.mk

VPATH = src/ src/bootloader

# C sources
C = \
	ata.c \
	clk.c \
	ext2.c \
	idt.c \
	init.c \
	intr.c \
	io.c \
	kbd.c \
	kmain.c \
	kmalloc.c \
	kprintf.c \
	pmm.c \
	proc.c \
	pq.c \
	queue.c \
	sched.c \
	sem.c \
	syscall.c \
	tty.c \
	vfs.c \
	vmm.c 

# asm sources
ASM = \
	ctxsw.s \
	intr.s \
	start.s \
	enter_usermode.s

USER = \
	user/ls/ls.o

OBJ = $(addprefix bin/, $(C:.c=.c.o) $(ASM:.s=.s.o))

all: libs maestro.bin bootloader img user

maestro.bin: $(OBJ) $(USER)
	$(LD) -o $@ $^ $(LDFLAGS)

stage1.bin: stage1.s
	$(AS) -i src/bootloader -f bin $< -o $@
	dd if=stage1.bin of=disk.img conv=notrunc

stage2.bin: stage2.s
	$(AS) -i src/bootloader -f bin $< -o $@
	e2cp stage2.bin disk.img:/

bin/%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

bin/%.s.o: %.s
	$(AS) -f elf32 $< -o $@

libs:
	$(MAKE) -C lib

user:
	$(MAKE) -C user

disk:
	meta/make_disk.sh

toolchain:
	meta/make_toolchain.sh

bootloader: stage1.bin stage2.bin

img: maestro.bin
	e2cp maestro.bin disk.img:/

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
