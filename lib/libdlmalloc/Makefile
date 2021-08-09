CC = gcc
AR = ar
CFLAGS = -std=gnu99 -march=i686 -m32 -fno-stack-protector -fno-pie -ffreestanding -nostdlib -Wall -Wextra $(INCLUDE)
INCLUDE = -I ../../include
LIB = libdlmalloc.a
BUILDDIR = $(addprefix $(TOPDIR), $(LIB))

SOURCES = malloc.c

OBJ = $(SOURCES:.c=.o)

all: $(LIB)

libdlmalloc.a: $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o $(BUILDDIR)