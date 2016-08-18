PA Compression Test
===================

Requirements
------------

1. Qt 5
2. Windows Installer XML 3.7+

Instructions
------------

0. Make sure that WIX compiler and linker are in your command line path
1. Compile the project:
  a. RsaToolbox
  b. QuaZip
2. Copy the following files:
  - Qt5.dll
  - quazip.dll
  - rsvisa32.dll
3. Run `create_installer.bat`  

The msi installer `R&S PA Compression Test X.Y.Z.msi` is created in the Installer/ folder
