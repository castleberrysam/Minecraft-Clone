#!/bin/bash

LIN_CMAKE=cmake
WIN_CMAKE=i686-w64-mingw32-cmake
#OSX_CMAKE=

mkdir -p include/linux/
mkdir -p lib/linux/
mkdir -p include/win/
mkdir -p lib/win/
#mkdir -p include/osx/
#mkdir -p lib/osx/

git clone git://git.sv.gnu.org/freetype/freetype2.git
mkdir freetype2/build
cd freetype2/build

# freetype for linux
$LIN_CMAKE -DBUILD_SHARED_LIBS=ON ..
make -j5
cp -r ../include/* ../../include/linux/
cp -r include/* ../../include/linux/
cp -d libfreetype.so* ../../lib/linux/
rm -rf *

# freetype for win
$WIN_CMAKE -DBUILD_SHARED_LIBS=ON ..
make -j5
cp -r ../include/* ../../include/win/
cp -r include/* ../../include/win/
cp libfreetype.dll* ../../lib/win/
rm -rf *

# freetype for osx
#$OSX_CMAKE -DBUILD_SHARED_LIBS=ON ..
#make -j5
#cp -r ../include/* ../../include/osx/
#cp -r include/* ../../include/osx/
# (copy the libraries)

cd ../..
rm -rf freetype2/

git clone https://github.com/nigels-com/glew.git
cd glew/
make extensions
cd build/

# glew for linux
$LIN_CMAKE ./cmake/
make -j5
cp -r ../include/* ../../include/linux/
cp lib/libGLEW.so ../../lib/linux/
rm -rf bin/ CMakeCache.txt CMakeFiles/ cmake_install.cmake lib/ Makefile

# glew for windows
$WIN_CMAKE ./cmake/
make -j5
cp -r ../include/* ../../include/win/
cp lib/libglew32.dll.a ../../lib/win/
cp bin/glew32.dll ../../lib/win/
rm -rf bin/ CMakeCache.txt CMakeFiles/ cmake_install.cmake lib/ Makefile

# glew for osx
#$OSX_CMAKE -DBUILD_FRAMEWORK=ON ./cmake/
#make -j5
#cp -r ../include/* ../../include/osx/
# (copy the libraries)

cd ../..
rm -rf glew/

git clone https://github.com/glfw/glfw.git
mkdir glfw/build/
cd glfw/build/

# glfw for linux
$LIN_CMAKE -DBUILD_SHARED_LIBS=ON ..
make -j5
cp -r ../include/* ../../include/linux/
cp -d src/libglfw.so* ../../lib/linux/
rm -rf *

# glfw for windows
$WIN_CMAKE -DBUILD_SHARED_LIBS=ON ..
make -j5
cp -r ../include/* ../../include/win/
cp src/glfw3.dll ../../lib/win/
cp src/libglfw3dll.a ../../lib/win/
rm -rf *

# glfw for osx
#$OSX_CMAKE ..
#make -j5
#cp -r ../include/* ../../include/osx/
# (copy the libraries)

cd ../..
rm -rf glfw/

git clone https://github.com/cxong/tinydir
cp tinydir/tinydir.h include/linux/
cp tinydir/tinydir.h include/win/
#cp tinydir/tinydir.h include/osx/

rm -rf tinydir/
