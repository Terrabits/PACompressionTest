# PA Compression Test

## Requirements

2. Desktop Qt 5.15.2 MinGW 64-bit
3. Conan 1.53.0
3. Windows Installer XML 3.11

## Setup

### Conan Install

```shell
# from project root directory
cd build
conan install .. --build=missing
```

This should generate several conan files in `build/`, including `conanbuildinfo.cmake`.

## Build

After conan install, the project can be built from Qt Creator.

1. Open the project in Qt Creator
2. When prompted, select kit `Desktop Qt 5.15.2 MinGW 64-bit` and choose your desired build locations.
3. On the bottom-left, choose a `Release` build.
4. From the file menu, click `Build`, then `Build All Projects`

You will need artifacts from the build. See the `Distribute` section below for required files.

## Distribute

Copy the following files to `Installer/ProgramFiles/Rohde-Schwarz/PA Compression Test/`.

- `platforms/qwindows.dll`
- `d3d_compiler47.dll`
- `libEGL.dll`
- `libgcc_s_seh-1.dll`
- `libGLESv2.dll`
- `libqcustomplot.dll`
- `libquazip1-qt5.dll`
- `libstdc++-6.dll`
- `libwinpthread-1.dll`
- `opengl32sw.dll`
- `PACompressionTest.exe`
- `Qt5Core.dll`
- `Qt5Gui.dll`
- `Qt5Network.dll`
- `Qt5PrintSupport.dll`
- `Qt5Test.dll`
- `Qt5Widgets.dll`
- `RsVisa64.dll`

Run `Installer/create_installer.bat` to create an MSI installer file.
