SRC = \
	init.c

OBJ = $(SRC:.c=.o)

all: init

init: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)
	e2cp init ../../disk.img:/bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f init *.o
