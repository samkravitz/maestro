include ../../config.mk

INCLUDE = -I ../../include
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
	rm -f *.o $(LIB)