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


QT      += core gui widgets printsupport

TARGET = RsaPaCompressionTest
TEMPLATE = app

include(RsaToolbox/rsatoolbox.pri)
include(RsaToolbox/QuaZip/quazip.pri)
include(TracesWidget/traceswidget.pri)
SOURCES +=  main.cpp \
            mainwindow.cpp \
            RunSweeps.cpp \
            gettracename.cpp \
            axis_settings.cpp \
            getCalibration.cpp \
            MeasurementData.cpp \
            ProcessTrace.cpp \
            PlotWidget/PlotWidget.cpp
INCLUDEPATH += $$PWD \
               $$PWD/PlotWidget
HEADERS  += Settings.h \
            mainwindow.h \
            RunSweeps.h \
            gettracename.h \
            axis_settings.h \
            getCalibration.h \
            MeasurementData.h \
            ProcessTrace.h \
            PlotWidget/PlotWidget.h
FORMS    += mainwindow.ui \
            gettracename.ui \
            axis_settings.ui \
            getCalibration.ui \
            PlotWidget/PlotWidget.ui

RESOURCES += Resources.qrc

OTHER_FILES += VS2010Resources.rc \
               DEBUG_LOGFILE.txt

DEFINES += SOURCE_DIR=\\\"$$PWD\\\"
CONFIG(debug, debug|release):DEFINES += DEBUG_MODE

win32: RC_FILE = VS2010Resources.rc
