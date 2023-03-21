CFLAGS = \
	-std=gnu99 \
	-ffreestanding \
	-nostdlib \
	-Wall \
	-Wextra \
	-mcmodel=large \
	-mno-red-zone \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-I ../../lib/libc/include

LDFLAGS = \
	-L ../../lib/libc \
	-lc

all: user_progs

user_progs:
	$(MAKE) -C ls
	$(MAKE) -C msh

PHONY: clean
clean:
	$(MAKE) -C ls clean
	$(MAKE) -C msh clean
