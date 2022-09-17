#-------------------------------------------------
#
# Project  : PACompressionTest
# App name : R&S PA Compression Test
#
#
# RsaToolbox Application
# Template : Rsa App
#            Version ?
#
# (C) Rohde & Schwarz (North) America
#
#-------------------------------------------------


QT      += core gui widgets

TARGET = PACompressionTest
TEMPLATE = app

include($$PWD/../Core/core.pri)
include($$PWD/../RsaToolbox/rsatoolbox.pri)
include($$PWD/../RsaToolbox/QuaZIP/quazip.pri)

HEADERS     += mainwindow.h
INCLUDEPATH += $$PWD
SOURCES     += main.cpp \
               mainwindow.cpp
FORMS       += mainwindow.ui
RESOURCES   += Resources.qrc
OTHER_FILES += VS2010Resources.rc \
               DEBUG_LOGFILE.txt
win32:      RC_FILE = VS2010Resources.rc

DEFINES += SOURCE_DIR=\\\"$$PWD\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE
