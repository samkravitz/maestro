CFLAGS = \
	-std=gnu99 \
	-ffreestanding \
	-nostdlib \
	-Wall \
	-Wextra \
	-I ../../lib/libc

LDFLAGS = \
	-L ../../lib/libc \
	-lc

all: user_progs

user_progs:
	$(MAKE) -C ls

PHONY: clean
clean:
	$(MAKE) -C ls clean
