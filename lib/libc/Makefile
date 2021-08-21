CC = gcc
CFLAGS = -std=c99 -march=i686 -m32 -fno-pie -fno-stack-protector -ffreestanding -nostdlib -O2 -Wall -Wextra -I $(INCLUDE)
INCLUDE = ../../include

SOURCES = \
	stdlib.c \
	string.c

OBJ = $(SOURCES:.c=.o)

all: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o