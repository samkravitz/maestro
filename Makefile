AS = nasm
CC = gcc
CFLAGS = -std=c99 -march=i386 -m32 -fno-stack-protector -fno-pie -ffreestanding -nostdlib -O2 -Wall -Wextra -I $(INCLUDE) -I lib/libc
LDFLAGS = -L lib/libc -l:libc.a
INCLUDE = include/
VPATH = src/

C_SOURCES = \
	gdt.c \
	idt.c \
	inittab.c \
	io.c \
	klog.c \
	kmain.c \
	txtmode.c

ASM_SOURCES = boot.s

OBJ = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)

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
	qemu-system-i386 -fda maestro.bin


.PHONY: clean
clean:
	rm -rf *.o *.bin
	$(MAKE) -C lib clean