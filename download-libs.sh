#!/bin/bash

LIN_CMAKE=cmake
WIN_CMAKE=i686-w64-mingw32-cmake
#OSX_CMAKE=cmake

if [ ${LIN_CMAKE+x} ]; then
    if [ ! "$($LIN_CMAKE --version)" ]; then exit 1; fi
    mkdir -p include/linux/
    mkdir -p lib/linux/
fi
if [ ${WIN_CMAKE+x} ]; then
    if [ ! "$($WIN_CMAKE --version)" ]; then exit 1; fi
    mkdir -p include/win/
    mkdir -p lib/win/
fi
if [ ${OSX_CMAKE+x} ]; then
    if [ ! "$($OSX_CMAKE --version)" ]; then exit 1; fi
    mkdir -p include/osx/
    mkdir -p lib/osx/
fi

git clone git://git.sv.gnu.org/freetype/freetype2.git
mkdir freetype2/build
cd freetype2/build

# freetype for linux
if [ ${LIN_CMAKE+x} ]; then
    $LIN_CMAKE -DBUILD_SHARED_LIBS=ON ..
    make -j5
    cp -r ../include/* ../../include/linux/
    cp -r include/* ../../include/linux/
    cp -d libfreetype.so* ../../lib/linux/
    rm -rf *
fi

# freetype for win
if [ ${WIN_CMAKE+x} ]; then
    $WIN_CMAKE -DBUILD_SHARED_LIBS=ON ..
    make -j5
    cp -r ../include/* ../../include/win/
    cp -r include/* ../../include/win/
    cp libfreetype.dll* ../../lib/win/
    rm -rf *
fi

# freetype for osx
if [ ${OSX_CMAKE+x} ]; then
    $OSX_CMAKE -DBUILD_SHARED_LIBS=ON ..
    make -j5
    cp -R ../include/ ../../include/osx/
    cp -R include/ ../../include/osx/
    find . ! -name libfreetype.\* -delete
    cp -R . ../../lib/osx/
    rm -rf *
fi

cd ../..
rm -rf freetype2/

git clone https://github.com/nigels-com/glew.git
cd glew/
make extensions
cd build/

# glew for linux
if [ ${LIN_CMAKE+x} ]; then
    $LIN_CMAKE ./cmake/
    make -j5
    cp -r ../include/* ../../include/linux/
    cp lib/libGLEW.so ../../lib/linux/
    rm -rf bin/ CMakeCache.txt CMakeFiles/ cmake_install.cmake lib/ Makefile
fi

# glew for windows
if [ ${WIN_CMAKE+x} ]; then
    $WIN_CMAKE ./cmake/
    make -j5
    cp -r ../include/* ../../include/win/
    cp lib/libglew32.dll.a ../../lib/win/
    cp bin/glew32.dll ../../lib/win/
    rm -rf bin/ CMakeCache.txt CMakeFiles/ cmake_install.cmake lib/ Makefile
fi

# glew for osx
if [ ${OSX_CMAKE+x} ]; then
    cd ..
    make glew.lib
    cp -R include/ ../include/osx/
    rm lib/libGLEW.a
    cp -R lib/ ../lib/osx/
    make clean
    cd build/
fi

cd ../..
rm -rf glew/

git clone https://github.com/glfw/glfw.git
mkdir glfw/build/
cd glfw/build/

# glfw for linux
if [ ${LIN_CMAKE+x} ]; then
    $LIN_CMAKE -DBUILD_SHARED_LIBS=ON ..
    make -j5
    cp -r ../include/* ../../include/linux/
    cp -d src/libglfw.so* ../../lib/linux/
    rm -rf *
fi

# glfw for windows
if [ ${WIN_CMAKE+x} ]; then
    $WIN_CMAKE -DBUILD_SHARED_LIBS=ON ..
    make -j5
    cp -r ../include/* ../../include/win/
    cp src/glfw3.dll ../../lib/win/
    cp src/libglfw3dll.a ../../lib/win/
    rm -rf *
fi

# glfw for osx
if [ ${OSX_CMAKE+x} ]; then
    $OSX_CMAKE -DBUILD_SHARED_LIBS=ON ..
    make -j5
    cp -R ../include/ ../../include/osx/
    cd src/
    find . ! -name libglfw.\* -delete
    cp -R . ../../../lib/osx/
    cd ..
    rm -rf *
fi

cd ../..
rm -rf glfw/

git clone https://github.com/cxong/tinydir

if [ ${LIN_CMAKE+x} ]; then cp tinydir/tinydir.h include/linux/; fi
if [ ${WIN_CMAKE+x} ]; then cp tinydir/tinydir.h include/win/; fi
if [ ${OSX_CMAKE+x} ]; then cp tinydir/tinydir.h include/osx/; fi

rm -rf tinydir/
