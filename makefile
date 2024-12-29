TARGET=main

CFLAGS=-Wall -Wextra -ggdb

SRC=main.c

RAYLIB_INC=-I./raylib/build/raylib/include/
RAYLIB_LIB=-L./raylib/build/raylib/ -lraylib -Wl,-rpath=./raylib/build/raylib/ 

MSL_INC=./msl/graph.h ./msl/array.h

build/${TARGET}: ${SRC} ${MSL_INC} rayext.h | build/
	cc ${RAYLIB_INC} ${RAYLIB_LIB} ${CFLAGS} $< -o $@

build/:
	mkdir build
