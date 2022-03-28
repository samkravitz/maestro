CC = gcc
CXX = g++
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

CXXFLAGS = \
	-std=c++17 \
	-m32 \
	-fno-pie \
	-fno-stack-protector \
	-fno-exceptions \
	-fno-rtti \
	-ffreestanding \
	-nostdlib \
	-O2 \
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
	-I lib/mxx
