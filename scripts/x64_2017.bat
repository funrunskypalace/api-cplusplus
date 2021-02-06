@echo off
set VCPKG_VER=x64-windows
set VCPKG_ROOT=d:/dev/vcpkg

set "var=%cd%"
cd %VCPKG_ROOT% 
rem realremove vcpkg integrate install
rem rem realremove vcpkg install c-ares:x64-windows
rem realremove vcpkg install tbb:x64-windows
rem realremove vcpkg install zlib:x64-windows
rem realremove vcpkg install xxhash:x64-windows
rem rem realremove vcpkg install abseil:x64-windows
rem realremove vcpkg install nng:x64-windows
rem do not !!! rem realremove vcpkg install libevent:x64-windows
rem realremove vcpkg install yaml-cpp:x64-windows
rem realremove vcpkg install tclap:x64-windows
rem realremove vcpkg install gtest:x64-windows
rem realremove vcpkg install nlohmann-json:x64-windows
rem realremove vcpkg install rapidjson:x64-windows
cd %var%
cd ..
copy /Y CMakeLists-template-win64.txt CMakeLists.txt
mkdir app_home
cd app_home
mkdir bin
cd bin
mkdir config
cd ..
mkdir config
cd ..
mkdir build
cd build
mkdir x64windows
cd x64windows
rem del CMakeCache.txt
rem -Dgtest_force_shared_crt=${GTEST_FORCE_SHARED_CRT} 
cmake ../.. -G "Visual Studio 15 2017 Win64" -T v141,host=x64 -DPreferredToolArchitecture=x64
cd ../../scripts
@echo on
