

QT += core gui widgets
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = $$PWD
TARGET = Core
CONFIG(debug, debug|release): TARGET = $$join(TARGET,,,d)

include($$PWD/../RsaToolbox/rsatoolbox.pri)
include($$PWD/../RsaToolbox/QuaZip/quazip.pri)

include($$PWD/Traces/traces.pri)
include($$PWD/MeasureThread/measurethread.pri)
include($$PWD/SpeedTracker/speedtracker.pri)

HEADERS     += Settings.h \
               MeasurementData.h \
               MeasurementSettings.h \
               ProcessTrace.h
INCLUDEPATH += $$PWD
SOURCES     += MeasurementData.cpp \
               MeasurementSettings.cpp \
               ProcessTrace.cpp
#FORMS       += .ui
#RESOURCES   += .qrc
OTHER_FILES += ../Documentation/GCA.csv

DEFINES     += SOURCE_DIR=\\\"$$PWD/\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
