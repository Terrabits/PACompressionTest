@echo off
SET "ROOT_DIR=%~dp0.."

setlocal
cd /d "%ROOT_DIR%"


REM make directories
mkdir "build" >nul 2>&1
mkdir "build\Desktop_Qt_5_15_2_MinGW_64_bit"         >nul 2>&1
mkdir "build\Desktop_Qt_5_15_2_MinGW_64_bit\Release" >nul 2>&1

REM add tools to path:
REM gcc, g++, cmake, ninja, WiX
set "PATH=%PATH%;C:\Qt\Tools\mingw810_64\bin"
set "PATH=%PATH%;C:\Qt\Tools\CMake_64\bin"
set "PATH=%PATH%;C:\Qt\Tools\Ninja"
set "PATH=%PATH%;C:\Program Files (x86)\WiX Toolset v3.11\bin"

REM prepare for build
cmake -S . ^
      -B "build\Desktop_Qt_5_15_2_MinGW_64_bit\Release"  ^
      "-DCMAKE_GENERATOR:STRING=Ninja"                   ^
      "-DCMAKE_BUILD_TYPE:STRING=Release"                ^
      "-DCMAKE_PROJECT_INCLUDE_BEFORE:FILEPATH=C:/Qt/Tools/QtCreator/share/qtcreator/package-manager/auto-setup.cmake" ^
      "-DQT_QMAKE_EXECUTABLE:FILEPATH=C:/Qt/5.15.2/mingw81_64/bin/qmake.exe" ^
      "-DCMAKE_PREFIX_PATH:PATH=C:/Qt/5.15.2/mingw81_64" ^
      "-DCMAKE_C_COMPILER:FILEPATH=C:/Qt/Tools/mingw810_64/bin/gcc.exe"   ^
      "-DCMAKE_CXX_COMPILER:FILEPATH=C:/Qt/Tools/mingw810_64/bin/g++.exe" ^
      "-DCMAKE_CXX_FLAGS_INIT:STRING="

REM build
cmake --build "build/Desktop_Qt_5_15_2_MinGW_64_bit/Release" --target all
