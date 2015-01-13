#-------------------------------------------------
# 
# Project  : RsaPaCompressionTest
# App name : RSA PA Compression Test
# 
# 
# RsaToolbox Application
# Template : Rsa App
#            Version 1.0.0.0
# 
# (C) Rohde & Schwarz America
# 
#-------------------------------------------------


QT      += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = RsaPaCompressionTest
TEMPLATE = app

include(RsaToolbox/rsatoolbox.pri)
SOURCES +=  main.cpp \
            mainwindow.cpp \
            RunSweeps.cpp \
            gettracename.cpp \
            axis_settings.cpp

HEADERS  += Settings.h \
            mainwindow.h \
            RunSweeps.h \
            gettracename.h \
            axis_settings.h

INCLUDEPATH += ./RsaToolbox/\
               ./QCustomPlot/

FORMS    += mainwindow.ui \
            gettracename.ui \
            axis_settings.ui

RESOURCES += Resources.qrc

OTHER_FILES += VS2010Resources.rc \
               DEBUG_LOGFILE.txt

DEFINES += QCUSTOMPLOT_USE_LIBRARY
DEFINES += SOURCE_DIR=\\\"$$PWD\\\"
CONFIG(debug, debug|release):DEFINES += LOCAL_LOG

win32: RC_FILE = VS2010Resources.rc
win32: LIBS += -L$$PWD/ -lRsaToolbox/rsib32

# Link with debug version of qcustomplot if compiling in debug mode, else with release library:
CONFIG(debug, release|debug) {
  win32:QCPLIB = qcustomplotd1
  else: QCPLIB = qcustomplotd
} else {
  win32:QCPLIB = qcustomplot1
  else: QCPLIB = qcustomplot
}
LIBS += -lQCustomPlot/$$QCPLIB

win32: QMAKE_CXXFLAGS += /D_CRT_SECURE_NO_WARNINGS
