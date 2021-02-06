#!/usr/bin/env bash

set -eu
export CC=/usr/local/bin/gcc
export CXX=/usr/local/bin/g++
export PATH="$PATH:$(npm bin)"
export OSVER=linux
export OOSTYPE=OpenOrder
# call protoc 
export JSOUT_DIR=../source/commonjs/message
export APIDIR=../api
export SRCDIR=../source

cd ..
BASEDIR=$(pwd)
yes | cp -rf CMakeLists-template-linux.txt CMakeLists.txt
mkdir -p target/bin
mkdir -p target/config
mkdir -p target/lib
mkdir -p app_home/config
mkdir -p app_home/bin
mkdir -p build/x64linux
cd build/x64linux
# del CMakeCache.txt
cmake ../.. -DCMAKE_INSTALL_PREFIX=$BASEDIR/target -DCMAKE_BUILD_TYPE=Release -DWIN32MT=off -D_CRT_NO_TIME_T=on  -D_CRT_NO_TIME_T=off
cd ../../scripts
cd ../build/x64linux
make -j$(nproc)
make install
cd ../../scripts