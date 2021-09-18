CXX = g++
AR = ar
INCLUDE = -I ../../include -I ../libc
CXXFLAGS = -std=c++17 -static -m32 -fno-exceptions -fno-rtti -fno-pie -fno-stack-protector -ffreestanding -nostdlib -O2 -Wall -Wextra $(INCLUDE)
LIB = mxx.a

SOURCES = \
	mstring.cpp \

OBJ = $(SOURCES:.cpp=.o)

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o $(LIB)