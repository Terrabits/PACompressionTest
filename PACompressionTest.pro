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


QT      += core gui widgets printsupport

TARGET = PACompressionTest
TEMPLATE = app

include(RsaToolbox/rsatoolbox.pri)
include(RsaToolbox/QuaZip/quazip.pri)
include(TracesWidget/traceswidget.pri)
include(PlotWidget/plotwidget.pri)
SOURCES +=  main.cpp \
            mainwindow.cpp \
            RunSweeps.cpp \
            gettracename.cpp \
            axis_settings.cpp \
            getCalibration.cpp \
            MeasurementData.cpp \
            ProcessTrace.cpp
INCLUDEPATH += $$PWD
HEADERS  += Settings.h \
            mainwindow.h \
            RunSweeps.h \
            gettracename.h \
            axis_settings.h \
            getCalibration.h \
            MeasurementData.h \
            ProcessTrace.h
FORMS    += mainwindow.ui \
            gettracename.ui \
            axis_settings.ui \
            getCalibration.ui

RESOURCES += Resources.qrc

OTHER_FILES += VS2010Resources.rc \
               DEBUG_LOGFILE.txt

DEFINES += SOURCE_DIR=\\\"$$PWD\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE

win32: RC_FILE = VS2010Resources.rc

