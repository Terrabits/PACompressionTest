QT          += core gui widgets

INCLUDEPATH += $$PWD \
               $$PWD/Traces \
               $$PWD/MeasureThread \
               $$PWD/SpeedTracker
DEPENDPATH  += $$PWD \
               $$PWD/Traces \
               $$PWD/MeasureThread \
               $$PWD/SpeedTracker

win32 {
    # C++11 enum class warnings:
    QMAKE_CXXFLAGS += /wd4482

    LIBS += -L$$PWD
    CONFIG(debug, release|debug) {
        LIBS += -l$$PWD/Cored
    }
    else {
        LIBS += -l$$PWD/Core
    }
}
macx {
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    LIBS += -stdlib=libc++
    LIBS += -L$$PWD
    CONFIG(debug, release|debug) {
        LIBS += $$PWD/libCored.a
    }
    else {
        LIBS += $$PWD/libCore.a
    }
}
