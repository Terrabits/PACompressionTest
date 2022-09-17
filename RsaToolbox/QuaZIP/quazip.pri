

QT      += core
DEFINES += NOMINMAX

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD
LIBS        += -L$$PWD

CONFIG(debug, debug|release) {
  LIBS += -lquazipd
}
else {
  LIBS += -lquazip
}

include(ZLib/zlib.pri)
