cmake_minimum_required(VERSION 3.5)
project(PACompressionTest VERSION 2.2.0 LANGUAGES CXX)


# settings
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# debug settings
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_MODE")
add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# no warnings
add_compile_options(-w)

# conan config
include(${CMAKE_BINARY_DIR}/../../conanbuildinfo.cmake)
conan_basic_setup(PACompressionTest)

# packages
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# set project files
set(PROJECT_SOURCES
      helpers.hpp
      helpers.cpp
      Resources.qrc
      main.cpp
      mainwindow.cpp
      mainwindow.h
      mainwindow.ui
)

# windows only
if(win32)
  # resource file for executable icon
  set(PROJECT_SOURCES "${PROJECT_SOURCES} VS2010Resources.rc")
endif()

# build executable
add_executable(PACompressionTest ${PROJECT_SOURCES})

# includes
target_include_directories(PACompressionTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# linking
target_link_libraries(PACompressionTest PUBLIC Qt5::Widgets)
target_link_libraries(PACompressionTest PUBLIC Core)
target_link_libraries(PACompressionTest PUBLIC Logging)
target_link_libraries(PACompressionTest PUBLIC RsaToolbox)
target_link_libraries(PACompressionTest PUBLIC QuaZip::QuaZip)
