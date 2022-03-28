include ../../config.mk

INCLUDE = -I ../../include
PROG = msh.o

SOURCES = \
	msh.c

OBJ = $(SOURCES:.c=.o)

all: $(PROG)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o
