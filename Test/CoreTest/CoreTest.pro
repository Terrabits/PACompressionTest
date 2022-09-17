

QT        += core testlib
TEMPLATE   = app
TARGET     = CoreTest
CONFIG    -= app_bundle
CONFIG    += console \
             testcase

include($$PWD/../../Core/core.pri)
include($$PWD/../../RsaToolbox/rsatoolbox.pri)
include($$PWD/../../RsaToolbox/QuaZIP/quazip.pri)
HEADERS     += SafeFrequencySweepTest.h \
               SpeedTrackerTest.h \
               TraceSettingsTest.h \
               ProcessTraceTest.h \
               MeasureDataTest.h \
               HeaderTest.h
SOURCES     += main.cpp \
               SafeFrequencySweepTest.cpp \
               SpeedTrackerTest.cpp \
               TraceSettingsTest.cpp \
               ProcessTraceTest.cpp \
               MeasureDataTest.cpp \
               HeaderTest.cpp

DEFINES   += SOURCE_DIR=\\\"$$PWD/\\\"
