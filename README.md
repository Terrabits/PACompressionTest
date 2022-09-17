PA Compression Test
===================

Requirements
------------

1. MinGW 64-bit 8.1.0
2. Qt 5.15.2
3. Windows Installer XML 3.9+

Instructions
------------

0. Make sure that WIX compiler and linker are in your command line path
1. Open the project in Qt Creator
2. On first open:  
   Choose the following build locations:  
   - `.\build\Desktop_Qt_5_2_1_MSVC2010_32bit\Debug`  
   - `.\build\Desktop_Qt_5_2_1_MSVC2010_32bit\Release`
3. In the lower-left corner, choose release build
2. From the file menu, run: `Build -> Rebuild All`
2. In the command line, navigate to `.\Installer\` folder
3. Run `create_installer.bat`

The msi installer `.\Installer\R&S PA Compression Test <version>.msi` is generated.
