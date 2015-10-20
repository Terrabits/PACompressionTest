

QT += core gui widgets
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD
TARGET = Core
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

include($$PWD/../RsaToolbox/rsatoolbox.pri)
include($$PWD/../RsaToolbox/QuaZip/quazip.pri)
HEADERS     += Settings.h \
               MeasurementData.h \
    MeasureThread.h
INCLUDEPATH += $$PWD
SOURCES     += MeasurementData.cpp \
    MeasureThread.cpp
#FORMS       += .ui
#RESOURCES   += .qrc


DEFINES     += SOURCE_DIR=\\\"$$PWD/\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
