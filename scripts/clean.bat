@echo off
SET "ROOT_DIR=%~dp0.."

setlocal
cd /d "%ROOT_DIR%"


REM clean
rmdir /s /q build\Desktop_Qt_5_15_2_MinGW_64_bit >nul 2>&1
del         build\Product.wixobj                 >nul 2>&1
