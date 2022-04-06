TARGET = i686-elf
TOOLCHAIN_PREFIX = $(abspath toolchain)/$(TARGET)/bin/$(TARGET)
GCC_VERSION = 11.2.0

export CC = $(TOOLCHAIN_PREFIX)-gcc
export AS = nasm
export LD = $(TOOLCHAIN_PREFIX)-ld
export AR = $(TOOLCHAIN_PREFIX)-ar

export CFLAGS = \
	-std=gnu99 \
	-ffreestanding \
	-nostdlib \
	-Wall \
	-Wextra \
	$(INCLUDE)

export LDFLAGS = \
	-T linker.ld \
	-Map=maestro.map \
	-L lib/libc \
	-lc \
	-L toolchain/$(TARGET)/lib/gcc/$(TARGET)/$(GCC_VERSION) \
	-lgcc

export INCLUDE = \
	-I include \
	-I lib/libc
