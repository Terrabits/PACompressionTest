

QT        += core testlib
TEMPLATE   = app
TARGET     = UnitTests
CONFIG    -= app_bundle
CONFIG    += console \
             testcase

include(../Core/core.pri)
HEADERS     += FrequencySweepTest.h
SOURCES     += main.cpp \
               FrequencySweepTest.cpp
OTHER_FILES += FrequencySweepTest_Log.txt

DEFINES   += SOURCE_DIR=\\\"$$PWD/\\\"


