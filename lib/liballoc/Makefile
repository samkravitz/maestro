CC = gcc
AR = ar
CFLAGS = -std=gnu99 -march=i686 -m32 -fno-pie -fno-stack-protector -ffreestanding -nostdlib -O2 -Wall -Wextra -I $(INCLUDE)
INCLUDE = ../../include
LIB = liballoc.a
BUILDDIR = $(addprefix $(TOPDIR), $(LIB))

SOURCES = \
	liballoc.c \
	liballoc_hooks.c

OBJ = $(SOURCES:.c=.o)

all: $(LIB)

liballoc.a: $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o $(BUILDDIR)