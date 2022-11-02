cmake_minimum_required(VERSION 3.5)
project(CoreTest LANGUAGES CXX)


# settings
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set source dir
add_compile_definitions(SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

# no warnings
add_compile_options(-w)

# conan config
include(${CMAKE_BINARY_DIR}/../../conanbuildinfo.cmake)
conan_basic_setup(CoreTest)

# packages
find_package(Qt5 REQUIRED COMPONENTS Test)

# set project files
set(PROJECT_SOURCES
    HeaderTest.cpp
    HeaderTest.h
    main.cpp
    MeasureDataTest.cpp
    MeasureDataTest.h
    ProcessTraceTest.cpp
    ProcessTraceTest.h
    SafeFrequencySweepTest.cpp
    SafeFrequencySweepTest.h
    SpeedTrackerTest.cpp
    SpeedTrackerTest.h
    TraceSettingsTest.cpp
    TraceSettingsTest.h
)

# build executable
add_executable(CoreTest ${PROJECT_SOURCES})

# includes
target_include_directories(CoreTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# linking
target_link_libraries(CoreTest PUBLIC Logging)
target_link_libraries(CoreTest PUBLIC Core)
target_link_libraries(CoreTest PUBLIC Qt5::Test)
