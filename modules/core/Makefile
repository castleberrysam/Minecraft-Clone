LIN_CC = gcc
WIN_CC = i686-w64-mingw32-gcc
OSX_CC = 
LIN_LIBS = -lGL -lGLU -lGLEW -lglfw -lpng -lfreetype -lm
WIN_LIBS = -lopengl32 -lglu32 -lglew32.dll -lglfw3dll -lpng16 -lfreetype.dll -lm
OSX_LIBS = 
LIN_BIN = libcore.so
WIN_BIN = core.dll
OSX_BIN = 
ALL_FLAGS = -g -Wall -mtune=generic -fPIC
LIN_FLAGS = 
WIN_FLAGS = 
OSX_FLAGS = 
ALL_LINK = -shared
LIN_LINK = 
WIN_LINK = 
OSX_LINK = 

SRC_FILES := $(wildcard src/*.c)
LIN_BUILD := $(patsubst src/%.c, build/linux/%.o, $(SRC_FILES))
WIN_BUILD := $(patsubst src/%.c, build/win/%.o, $(SRC_FILES))
OSX_BUILD := $(patsubst src/%.c, build/osx/%.o, $(SRC_FILES))
all: linux
linux: $(LIN_BUILD) dist
	$(LIN_CC) $(LIN_LINK) $(ALL_LINK) -o dist/linux/$(LIN_BIN) ${LIN_BUILD} -L../../lib/linux/ $(LIN_LIBS)
win: $(WIN_BUILD) dist
	$(WIN_CC) $(WIN_LINK) $(ALL_LINK) -o dist/win/$(WIN_BIN) $(WIN_BUILD) -L../../lib/win/ $(WIN_LIBS)
osx: $(OSX_BUILD) dist
	$(OSX_CC) $(OSX_LINK) $(ALL_LINK) -o dist/osx/$(OSX_BIN) $(OSX_BUILD) -L../../lib/osx/ $(OSX_LIBS)
build/linux/%.o: src/%.c build
	$(LIN_CC) -c $(LIN_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/linux/ -I../../include/linux/
build/win/%.o: src/%.c build
	$(WIN_CC) -c $(WIN_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/win/ -I../../include/win/
build/osx/%.o: src/%.c build
	$(OSX_CC) -c $(OSX_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/osx/ -I../../include/osx/
build:
	mkdir -p build/linux/
	mkdir -p build/win/
	mkdir -p build/osx/
dist:
	mkdir -p dist/linux/
	cp -r res/ dist/linux/res/
	mkdir -p dist/win/
	cp -r res/ dist/win/res/
	mkdir -p dist/osx/
	cp -r res/ dist/osx/res/
clean:
	-rm -rf build/
	-rm -rf dist/
