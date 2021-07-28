CC = gcc
AR = ar
CFLAGS = -std=c99 -march=i386 -m32 -fno-pie -ffreestanding -nostdlib -O2 -Wall -Wextra
LIB = libc.a
BUILDDIR = $(addprefix $(TOPDIR), $(LIB))

SOURCES = \
	string.c

OBJ = $(SOURCES:.c=.o)

all: $(LIB)

libc.a: $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o $(BUILDDIR)