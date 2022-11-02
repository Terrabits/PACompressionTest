cmake_minimum_required(VERSION 3.5)
project(QCustomPlot VERSION 1.3.1 LANGUAGES CXX)

# files
set(PROJECT_SOURCES
      qcustomplot.cpp
      qcustomplot.h
)

# build settings
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# no warnings
add_compile_options(-w)

# packages
find_package(Qt5 REQUIRED COMPONENTS Widgets PrintSupport)

# define shared lib
add_library(qcustomplot SHARED ${PROJECT_SOURCES})
target_compile_definitions(qcustomplot PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY)
target_compile_definitions(qcustomplot INTERFACE QCUSTOMPLOT_USE_LIBRARY)
target_include_directories(qcustomplot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(qcustomplot PUBLIC Qt5::Widgets Qt5::PrintSupport)
