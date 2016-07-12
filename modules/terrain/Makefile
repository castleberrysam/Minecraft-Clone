LIN_CC = gcc
WIN_CC = i686-w64-mingw32-gcc
OSX_CC = 
LIN_LIBS = -lGL -lGLEW -lcore
WIN_LIBS = -lopengl32 -lglu32 -lglew32.dll -lcore
OSX_LIBS = 
LIN_BIN = libterrain.so
WIN_BIN = terrain.dll
OSX_BIN = 
ALL_FLAGS = -g -Wall -mtune=generic -Og -fPIC
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
all: linux win
linux: $(LIN_BUILD) dist
	$(LIN_CC) $(LIN_LINK) $(ALL_LINK) -o dist/linux/$(LIN_BIN) ${LIN_BUILD} -Llib/linux/ -L../../lib/linux/ -L../core/dist/linux/ $(LIN_LIBS)
win: $(WIN_BUILD) dist
	$(WIN_CC) $(WIN_LINK) $(ALL_LINK) -o dist/win/$(WIN_BIN) $(WIN_BUILD) -Llib/win/ -L../../lib/win/ -L../core/dist/win/ $(WIN_LIBS)
osx: $(OSX_BUILD) dist
	$(OSX_CC) $(OSX_LINK) $(ALL_LINK) -o dist/osx/$(OSX_BIN) $(OSX_BUILD) -Llib/osx/ -L../../lib/osx/ -L../core/dist/osx/ $(OSX_LIBS)
build/linux/%.o: src/%.c build
	$(LIN_CC) -c $(LIN_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/linux/ -I../../include/linux/ -I../core/src/
build/win/%.o: src/%.c build
	$(WIN_CC) -c $(WIN_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/win/ -I../../include/win/ -I../core/src/
build/osx/%.o: src/%.c build
	$(OSX_CC) -c $(OSX_FLAGS) $(ALL_FLAGS) -o $@ $< -Iinclude/osx/ -I../../include/osx/ -I../core/src/
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
