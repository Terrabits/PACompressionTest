

QT        += core testlib
TEMPLATE   = app
TARGET     = UnitTests
CONFIG    -= app_bundle
CONFIG    += console \
             testcase

include(../Core/core.pri)
HEADERS     += SafeFrequencySweepTest.h
SOURCES     += main.cpp \
               SafeFrequencySweepTest.cpp
OTHER_FILES += SafeFrequencySweepTest_Log.txt

DEFINES   += SOURCE_DIR=\\\"$$PWD/\\\"


