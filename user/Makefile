CFLAGS = \
	-std=gnu99 \
	-ffreestanding \
	-nostdlib \
	-Wall \
	-Wextra \
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
