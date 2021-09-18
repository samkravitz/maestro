CXX = g++
AR = ar
INCLUDE = ../../include
CXXFLAGS = -std=c++17 -static -m32 -fno-pie -fno-stack-protector -ffreestanding -nostdlib -O2 -Wall -Wextra -I $(INCLUDE)
LIB = mxx.a

SOURCES = \
	test.cpp \

OBJ = $(SOURCES:.cpp=.o)

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) crv $(LIB) $(OBJ)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o $(LIB)