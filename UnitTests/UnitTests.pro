

QT        += core testlib
TEMPLATE   = app
TARGET     = UnitTests
CONFIG    -= app_bundle
CONFIG    += console \
             testcase

include(../Core/core.pri)
HEADERS     += SafeFrequencySweepTest.h \
               SpeedTrackerTest.h \
               TraceSettingsTest.h \
               ProcessTraceTest.h \
               MeasureDataTest.h
SOURCES     += main.cpp \
               SafeFrequencySweepTest.cpp \
               SpeedTrackerTest.cpp \
               TraceSettingsTest.cpp \
               ProcessTraceTest.cpp \
               MeasureDataTest.cpp
OTHER_FILES += SafeFrequencySweepTest_Log.txt

DEFINES   += SOURCE_DIR=\\\"$$PWD/\\\"


