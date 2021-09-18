CC = gcc
AR = ar
INCLUDE = ../../include
CFLAGS = -std=c99 -march=i686 -m32 -fno-pie -fno-stack-protector -ffreestanding -nostdlib -O2 -Wall -Wextra -I $(INCLUDE)
LIB = libc.a

SOURCES = \
	stdlib.c \
	string.c \

OBJ = $(SOURCES:.c=.o)

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o $(LIB)