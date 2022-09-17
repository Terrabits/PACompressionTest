

QT          += core gui widgets

include($$PWD/../RsaToolbox/rsatoolbox.pri)
include($$PWD/../RsaToolbox/QuaZip/quazip.pri)

INCLUDEPATH += $$PWD \
               $$PWD/Traces \
               $$PWD/MeasureThread \
               $$PWD/SpeedTracker
DEPENDPATH  += $$PWD \
               $$PWD/Traces \
               $$PWD/MeasureThread \
               $$PWD/SpeedTracker

LIBS += -L$$PWD

CONFIG(debug, release|debug) {
   LIBS += -lCored
}
else {
   LIBS += -lCore
}

macx {
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    LIBS += -stdlib=libc++
}
