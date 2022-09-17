#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T21:27:35
#
#-------------------------------------------------

QT       += core gui widgets
TARGET = PlotUiTest
TEMPLATE = app

include($$PWD/../../Core/core.pri)
include($$PWD/../../RsaToolbox/rsatoolbox.pri)
include($$PWD/../../RsaToolbox/QuaZIP/quazip.pri)

HEADERS     += mainwindow.h
SOURCES     += main.cpp \
               mainwindow.cpp
FORMS       += mainwindow.ui
OTHER_FILES += DEBUG_LOGFILE.txt

DEFINES     += SOURCE_DIR=\\\"$$PWD\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE
