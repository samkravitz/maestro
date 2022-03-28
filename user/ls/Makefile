include ../../config.mk

INCLUDE = -I ../../include
PROG = ls.o

SOURCES = \
	ls.c

OBJ = $(SOURCES:.c=.o)

all: $(PROG)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o
