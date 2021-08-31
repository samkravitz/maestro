CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra
SRC = \
	pq.test.c \
	kmalloc.test.c \

TESTS = $(SRC:.c=.out)
INCLUDE = -I ../include

all: tests

tests: $(TESTS)

%.out: %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf *.out