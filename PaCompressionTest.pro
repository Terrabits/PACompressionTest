#-------------------------------------------------
# 
# Project  : PaCompressionTest
# App name : R&S PA Compression Test
# 
# 
# RsaToolbox
# Template : Rsa Vna Macro Without Ui
#            Version 2.0.0
# 
# (C) Rohde & Schwarz America
# 
#-------------------------------------------------


QT          += core gui widgets
TARGET       = PaCompressionTest
TEMPLATE     = app

include(RsaToolbox/rsatoolbox.pri)
#include(RsaToolbox/QuaZip/quazip.pri)
HEADERS     += Settings.h \
    LandingPage.h
INCLUDEPATH += $$PWD
SOURCES     += main.cpp \
    LandingPage.cpp
RESOURCES   += Resources.qrc
OTHER_FILES += DEBUG_LOGFILE.txt

# Executable Icon
win32: RC_FILE = VS2010Resources.rc

DEFINES     += SOURCE_DIR=\\\"$$PWD\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

FORMS += \
    LandingPage.ui

