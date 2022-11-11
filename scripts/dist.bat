@echo off
SET "ROOT_DIR=%~dp0.."

setlocal
cd /d "%ROOT_DIR%"


REM make directories
mkdir "build" >nul 2>&1
mkdir "dist"  >nul 2>&1
mkdir "Installer\ProgramFiles\Rohde-Schwarz\PA Compression Test"           >nul 2>&1
mkdir "Installer\ProgramFiles\Rohde-Schwarz\PA Compression Test\platforms" >nul 2>&1

REM copy bins
copy  "build\Desktop_Qt_5_15_2_MinGW_64_bit\Release\RsaToolbox\QCustomPlot\libqcustomplot.dll" "Installer\ProgramFiles\Rohde-Schwarz\PA Compression Test\"
copy  "build\Desktop_Qt_5_15_2_MinGW_64_bit\Release\RsaToolbox\QuaZIP\bin\libquazip1-qt5.dll"  "Installer\ProgramFiles\Rohde-Schwarz\PA Compression Test\"
copy  "build\Desktop_Qt_5_15_2_MinGW_64_bit\Release\Application\bin\PACompressionTest.exe"     "Installer\ProgramFiles\Rohde-Schwarz\PA Compression Test\"

REM create installer
cd Installer
candle.exe "Product.wxs"                   -ext WixUtilExtension -ext WixUIExtension -out "..\build\Product.wixobj"
light.exe  "..\build\Product.wixobj" -spdb -ext WixUtilExtension -ext WixUIExtension -out "..\dist\R&S PA Compression Test 2.3.0.msi"
