@echo off
set VCPKG_VER=x86-windows
set VCPKG_ROOT=d:/dev/vcpkg

set "var=%cd%"
cd %VCPKG_ROOT% 
rem realremove vcpkg integrate install
rem rem realremove vcpkg install c-ares:x86-windows
rem realremove vcpkg install tbb:x86-windows
rem realremove vcpkg install zlib:x86-windows
rem realremove vcpkg install xxhash:x86-windows
rem rem realremove vcpkg install abseil:x86-windows
rem realremove vcpkg install nng:x86-windows
rem do not !!! rem realremove vcpkg install libevent:x86-windows
rem realremove vcpkg install yaml-cpp:x86-windows
rem realremove vcpkg install tclap:x86-windows
rem realremove vcpkg install gtest:x86-windows
rem realremove vcpkg install nlohmann-json:x86-windows
rem realremove vcpkg install rapidjson:x86-windows
cd %var%
cd ..
copy /Y CMakeLists-template-win32.txt CMakeLists.txt
mkdir app_home
cd app_home
mkdir lib
mkdir bin
cd bin
mkdir config
cd ..
mkdir config
cd ..
mkdir build
cd build
mkdir x86windows
cd x86windows
rem del CMakeCache.txt
rem -Dgtest_force_shared_crt=${GTEST_FORCE_SHARED_CRT} 
cmake ../.. -G "Visual Studio 16 2019" -A win32 -T v142,host=x86 -DPreferredToolArchitecture=x86
cd ../../scripts
@echo on
