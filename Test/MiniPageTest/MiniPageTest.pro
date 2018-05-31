#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T21:27:35
#
#-------------------------------------------------

QT       += core gui widgets
TARGET = MiniPageTest
TEMPLATE = app

include($$PWD/../../lib/lib.pri)
include($$PWD/../../Core/core.pri)
SOURCES     += main.cpp

DEFINES     += SOURCE_DIR=\\\"$$PWD\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE
