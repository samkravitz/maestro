include ../../config.mk

INCLUDE = -I ../../include
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
	rm -f *.o $(LIB)