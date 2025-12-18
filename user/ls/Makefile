SRC = \
	ls.c

OBJ = $(SRC:.c=.o)

all: ls

ls: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)
	e2cp ls ../../disk.img:/bin

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f ls *.o
