LIB = libc.a libk.a

KFLAGS = $(CFLAGS) -I ../../include -D LIBK

# libc sources
C_SRC = \
	crt0.s \
	malloc.c \
	stdio.c \
	stdlib.c \
	string.c \
	syscall.c \
	syscall.s \
	unistd.c

#libk sources
K_SRC = \
	stdio.c \
	stdlib.c \
	string.c

C_OBJ = $(C_SRC:.c=.c.o) $(C_SRC:.s=.s.o)
K_OBJ = $(K_SRC:.c=.k.o)

all: $(LIB)

libc.a: $(C_OBJ)
	$(AR) rcs libc.a $(C_OBJ)

libk.a: $(K_OBJ)
	$(AR) rcs libk.a $(K_OBJ)

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.s
	$(AS) -f elf32 $< -o $@

%.k.o: %.c
	$(CC) $(KFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(LIB)