LIN_CC = gcc
WIN_CC = x86_64-pc-cygwin-gcc
OSX_CC = 
LIN_LIBS = -lGL -lGLU -lGLEW -lglfw -lpng -lfreetype -lm -lopenal -lvorbisfile
WIN_LIBS = -lopengl32 -lglu32 -lglew32 -lglfw3dll -lpng16 -lfreetype.dll -lm -lopenal -lvorbisfile
OSX_LIBS = 
LIN_BIN = libcore.so
WIN_BIN = core.dll
OSX_BIN = 
ALL_FLAGS = -g -Wall -mtune=generic -Og
LIN_FLAGS = -fPIC
WIN_FLAGS = -std=c11
OSX_FLAGS = -fPIC
ALL_LINK = -shared
LIN_LINK = 
WIN_LINK = 
OSX_LINK = 

SRC_FILES := $(wildcard src/*.c)
LIN_BUILD := $(patsubst src/%.c, build/linux/%.o, $(SRC_FILES))
WIN_BUILD := $(patsubst src/%.c, build/win/%.o, $(SRC_FILES))
OSX_BUILD := $(patsubst src/%.c, build/osx/%.o, $(SRC_FILES))
.PHONY: linux win osx clean
linux: $(LIN_BUILD) dist
	$(LIN_CC) $(LIN_LINK) $(ALL_LINK) -o dist/linux/$(LIN_BIN) ${LIN_BUILD} -L../../lib/linux/ $(LIN_LIBS)
win: $(WIN_BUILD) dist
	$(WIN_CC) $(WIN_LINK) $(ALL_LINK) -o dist/win/$(WIN_BIN) $(WIN_BUILD) -L../../lib/win/ $(WIN_LIBS)
osx: $(OSX_BUILD) dist
	$(OSX_CC) $(OSX_LINK) $(ALL_LINK) -o dist/osx/$(OSX_BIN) $(OSX_BUILD) -L../../lib/osx/ $(OSX_LIBS)
build/linux/%.o: src/%.c build
	$(LIN_CC) -c $(LIN_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/linux/ -I../../include/linux/
build/win/%.o: src/%.c build
	$(WIN_CC) -c $(WIN_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/win/ -I../../include/win/ -I/usr/include/freetype2/
build/osx/%.o: src/%.c build
	$(OSX_CC) -c $(OSX_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/osx/ -I../../include/osx/
build:
	mkdir -p build/linux/
	mkdir -p build/win/
	mkdir -p build/osx/
dist:
	mkdir -p dist/linux/
	mkdir -p dist/win/
	mkdir -p dist/osx/
clean:
	-rm -rf build/
	-rm -rf dist/
