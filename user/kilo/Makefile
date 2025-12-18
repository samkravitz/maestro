SRC = \
	kilo.c

OBJ = $(SRC:.c=.o)

all: kilo

kilo: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)
	e2cp kilo ../../disk.img:/bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f kilo *.o
