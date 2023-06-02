CC=cc
CFLAGS=-Wall -Wextra -O2 -g3
INC=-Iinclude -I/usr/local
GLFW_LIB=$(shell pkg-config --libs glfw3)
LIB=-lm $(GLFW_LIB) -lassimp -lGL
SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c,%.o,$(SRC))
BIN=doors_remake

default: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)
	rm -rf *.o

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< $(INC)

clean:
	rm -f $(OBJ) $(BIN)
