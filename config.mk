CC = gcc
AS = nasm
LD = ld
AR = ar

CFLAGS = \
	-std=gnu99 \
	-march=i686 \
	-m32 \
	-fno-stack-protector \
	-fno-pie \
	-ffreestanding \
	-nostdlib \
	-Wall \
	-Wextra \
	$(INCLUDE)

LDFLAGS = \
	-T linker.ld \
	-m elf_i386 \
	-Map=maestro.map \
	-L lib/libc \
	-lc

INCLUDE = \
	-I include \
	-I lib/libc \
