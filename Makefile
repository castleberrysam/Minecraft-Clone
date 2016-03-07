SRC_FILES := $(wildcard src/*.c)
BUILD_FILES := $(patsubst src/%.c, build/%.o, ${SRC_FILES})

all: ${BUILD_FILES} dist
	gcc -o dist/game ${BUILD_FILES} -Llib/ -lGLEW -lGL -lglut -lglib-2.0

build/%.o: src/%.c build
	gcc -c -o $@ $< -Iinclude/
build:
	mkdir build/
dist:
	mkdir dist/
clean:
	-rm -rf build/
	-rm -rf dist/
