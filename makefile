TARGET=main

CFLAGS=-Wall -Wextra -ggdb

SRC=main.c

RAYLIB_INC=-I./raylib/build/raylib/include/
RAYLIB_LIB=-L./raylib/build/raylib/ -lraylib -Wl,-rpath=./raylib/build/raylib/ 

MSL_INC=./msl/graph.h ./msl/array.h

all: build/ msl/ | build/${TARGET}
PHONY: all
 
build/${TARGET}: ${SRC} ${MSL_INC} rayext.h 
	cc ${RAYLIB_INC} ${RAYLIB_LIB} ${CFLAGS} $< -o $@

build/:
	mkdir build

msl/:
	git clone https://github.com/ishay320/msl.git
