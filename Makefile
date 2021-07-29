AS = nasm
CC = gcc
CFLAGS = -std=c99 -march=i386 -m32 -fno-stack-protector -fno-pie -ffreestanding -nostdlib -O2 -Wall -Wextra -I $(INCLUDE) -I lib/libc
LDFLAGS = -L lib/libc -l:libc.a
INCLUDE = include/
VPATH = src/

C_SOURCES = \
	gdt.c \
	idt.c \
	io.c \
	kmain.c \
	txtmode.c

ASM_SOURCES = boot.s

OBJ = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)

all: libs kernel.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) -f elf32 $< -o $@

kernel.bin: $(OBJ) $(LIBS)
	$(CC) $(CFLAGS) -T linker.ld -o $@  $^ -lgcc $(LDFLAGS)

libs:
	$(MAKE) -C lib

.PHONY: start
start:
	qemu-system-i386 -fda kernel.bin


.PHONY: clean
clean:
	rm -rf *.o *.bin
	$(MAKE) -C lib clean