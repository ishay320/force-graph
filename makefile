CFLAGS=-Wall -Wextra
SRC=main.c
TARGET=main

RAYLIB_INC=-I./raylib/build/raylib/include/
RAYLIB_LIB=-L./raylib/build/raylib/ -lraylib -Wl,-rpath=./raylib/build/raylib/ 

build/${TARGET}: ${SRC} | build/
	cc ${RAYLIB_INC} ${RAYLIB_LIB} ${CFLAGS} $< -o $@

build/:
	mkdir build
