cmake_minimum_required(VERSION 3.23)
project(Logging)

# settings
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# conan config
include(${CMAKE_BINARY_DIR}/../../conanbuildinfo.cmake)
conan_basic_setup(Logging)

# project files
set(PROJECT_SOURCES
      logging.hpp
      logging.cpp
)

# build static library
add_library(Logging STATIC ${PROJECT_SOURCES})

# includes
target_include_directories(Logging PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# linking
target_link_libraries(Logging PUBLIC ${CONAN_LIBS})
