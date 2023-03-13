SRC = \
	msh.c

OBJ = $(SRC:.c=.o)

all: msh

msh: $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)
	e2cp msh ../../disk.img:/

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f msh *.o
