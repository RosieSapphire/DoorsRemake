CC=cc
CFLAGS=-Wall -Wextra -O2
INC=-Iinclude -Ideps/sightseer/include -Ideps/rmath/include
GLFW_LIB=$(shell pkg-config --libs glfw3)
SIGHTSEER_LIB=-Ldeps/sightseer -lsightseer
RMATH_LIB=-Ldeps/rmath -lrmath
LIB=-lm $(GLFW_LIB) $(SIGHTSEER_LIB) $(RMATH_LIB) -lassimp
SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c,%.o,$(SRC))
BIN=funny_block_game

default: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)
	rm -rf *.o

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< $(INC)

clean:
	rm -f $(OBJ) $(BIN)
