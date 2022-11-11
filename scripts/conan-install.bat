@echo off
SET "ROOT_DIR=%~dp0.."

setlocal
cd /d "%ROOT_DIR%"


REM install conan packages
mkdir build >nul 2>&1
cd build
conan install .. --build=missing
