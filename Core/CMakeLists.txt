cmake_minimum_required(VERSION 3.5)
project(Core LANGUAGES CXX)


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
conan_basic_setup(Logging)

# packages
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# project files
set(PROJECT_SOURCES
      MeasureThread/FrequencySweep.cpp
      MeasureThread/FrequencySweep.h
      MeasureThread/MeasureThread.cpp
      MeasureThread/MeasureThread.h
      MeasureThread/SafeFrequencySweep.cpp
      MeasureThread/SafeFrequencySweep.h
      SpeedTracker/SpeedTracker.cpp
      SpeedTracker/SpeedTracker.h
      SpeedTracker/SpeedTrackerChild.cpp
      SpeedTracker/SpeedTrackerChild.h
      Traces/TraceSettings.cpp
      Traces/TraceSettings.h
      Traces/TraceSettingsDelegate.cpp
      Traces/TraceSettingsDelegate.h
      Traces/TraceSettingsModel.cpp
      Traces/TraceSettingsModel.h
      Traces/TracesWidget.cpp
      Traces/TracesWidget.h
      Traces/TracesWidget.ui
      CoreResources.qrc
      DragWidget.cpp
      DragWidget.h
      MeasurementData.cpp
      MeasurementData.h
      MeasurementSettings.cpp
      MeasurementSettings.h
      MiniPage.cpp
      MiniPage.h
      MiniPage.ui
      ProcessTrace.cpp
      ProcessTrace.h
      ProgressPage.cpp
      ProgressPage.h
      ProgressPage.ui
      Settings.h
      SettingsPage.cpp
      SettingsPage.h
      SettingsPage.ui
)

# build static lib
add_library(Core STATIC ${PROJECT_SOURCES})

# includes
target_include_directories(Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Core PUBLIC MeasureThread)
target_include_directories(Core PUBLIC SpeedTracker)
target_include_directories(Core PUBLIC Traces)

# linking
target_link_libraries(Core PUBLIC Logging)
target_link_libraries(Core PUBLIC RsaToolbox)
target_link_libraries(Core PUBLIC QuaZip::QuaZip)
target_link_libraries(Core PUBLIC Qt5::Widgets)
