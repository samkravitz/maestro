include ../../config.mk

INCLUDE = -I ../../include
LIB = libc.a

# C sources
C = \
	stdio.c \
	stdlib.c \
	string.c \
	syscall.c

# asm sources
ASM = syscall.s

OBJ = $(C:.c=.c.o) $(ASM:.s=.s.o)

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.s
	$(AS) -f elf32 $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(LIB)