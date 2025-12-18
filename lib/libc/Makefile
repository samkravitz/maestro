LIB = libc.a libk.a
CRT0 = bin/crt0.o

CFLAGS += -I include

KFLAGS = $(CFLAGS) -I ../../include -D LIBK

# libc sources
C_SRC = \
	$(wildcard src/*.c) \
	$(wildcard src/*.s) \
	$(wildcard src/**/*.c) \
	$(wildcard src/**/*.s)

#libk sources
K_SRC = \
	$(wildcard src/stdio/*.c) \
	$(wildcard src/stdlib/*.c) \
	$(wildcard src/string/*.c)

C_OBJ = $(C_SRC:.c=.c.o) $(C_SRC:.s=.s.o)
K_OBJ = $(K_SRC:.c=.k.o)

all: $(LIB) $(CRT0)

libc.a: $(C_OBJ)
	$(AR) rcs libc.a $(C_OBJ)

libk.a: $(K_OBJ)
	$(AR) rcs libk.a $(K_OBJ)

bin/crt0.o: src/crt0.s
	mkdir -p bin
	$(AS) -f elf32 $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.s.o: %.s
	$(AS) -f elf32 $< -o $@

%.k.o: %.c
	$(CC) $(KFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f src/**/*.o src/*.o $(LIB) bin/*.o
