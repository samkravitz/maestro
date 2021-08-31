CC = gcc
CFLAGS = -std=gnu99 -m32 -Wall -Wextra $(INCLUDE)
SRC = \
	pq.test.c \
	# kmalloc.test.c \

TESTS = $(SRC:.c=.out)
INCLUDE = -I ../include
VPATH = ../src/

all: tests

tests: $(TESTS)

%.out: %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf *.out