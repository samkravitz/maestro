SRC = \
	cat.c

OBJ = $(SRC:.c=.o)

all: cat

cat: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)
	e2cp cat ../../disk.img:/

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f cat *.o
