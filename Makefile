AS = nasm
CC = gcc
CFLAGS = -std=c99 -march=i386 -m32 -fno-pie -ffreestanding -nostdlib -O2 -Wall -Wextra -I $(INCLUDE)
INCLUDE = include/
VPATH = src/

C_SOURCES = \
	kmain.c \
	txtmode.c

ASM_SOURCES = boot.s

OBJ = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)

%.o: %.s
	$(AS) -f elf32 $< -o $@

all: kernel.bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: $(OBJ)
	$(CC) $(CFLAGS) -T linker.ld -o $@  $^ -lgcc

.PHONY: start
start:
	qemu-system-i386 -fda kernel.bin


.PHONY: clean
clean:
	rm -rf *.o *.bin