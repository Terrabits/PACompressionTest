#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T21:27:35
#
#-------------------------------------------------

QT       += core gui widgets
TARGET = MiniPageTest
TEMPLATE = app

include($$PWD/../../Core/core.pri)
include($$PWD/../../RsaToolbox/rsatoolbox.pri)
include($$PWD/../../RsaToolbox/QuaZIP/quazip.pri)

SOURCES     += main.cpp

DEFINES     += SOURCE_DIR=\\\"$$PWD\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE
