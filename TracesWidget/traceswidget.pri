

include(../RsaToolbox/rsatoolbox.pri)
HEADERS     += $$PWD/TraceSettings.h \
               $$PWD/TraceSettingsModel.h \
               $$PWD/TraceSettingsDelegate.h \
               $$PWD/TracesWidget.h
INCLUDEPATH += $$PWD
SOURCES     += $$PWD/TraceSettings.cpp \
               $$PWD/TraceSettingsModel.cpp \
               $$PWD/TraceSettingsDelegate.cpp \
               $$PWD/TracesWidget.cpp
FORMS       += $$PWD/TracesWidget.ui
