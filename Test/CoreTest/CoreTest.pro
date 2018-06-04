

QT        += core testlib
TEMPLATE   = app
TARGET     = CoreTest
CONFIG    -= app_bundle
CONFIG    += console \
             testcase

include($$PWD/../../lib/lib.pri)
include($$PWD/../../Core/core.pri)
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

DEFINES   += SOURCE_DIR=\\\"$$PWD/\\\"
