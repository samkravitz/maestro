CC = gcc
CFLAGS = -std=gnu99 -march=i686 -m32 -fno-stack-protector -fno-pie -ffreestanding -nostdlib -Wall -Wextra $(INCLUDE)
INCLUDE = -I ../../include

SOURCES = malloc.c

OBJ = $(SOURCES:.c=.o)

all: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o