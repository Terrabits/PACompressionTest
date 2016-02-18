

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
               ProcessTrace.h \
    SettingsPage.h \
    ProgressPage.h \
    MiniPage.h
INCLUDEPATH += $$PWD
SOURCES     += MeasurementData.cpp \
               MeasurementSettings.cpp \
               ProcessTrace.cpp \
    SettingsPage.cpp \
    ProgressPage.cpp \
    MiniPage.cpp
#FORMS       += .ui
#RESOURCES   += .qrc
OTHER_FILES += ../Documentation/GCA.csv

DEFINES     += SOURCE_DIR=\\\"$$PWD/\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

FORMS += \
    SettingsPage.ui \
    ProgressPage.ui \
    MiniPage.ui

RESOURCES += \
    CoreResources.qrc
