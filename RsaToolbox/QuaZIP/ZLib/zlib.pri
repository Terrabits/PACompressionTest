

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD
LIBS        += -L$$PWD
win32 {
    LIBS += -lzlib
}
macx {
    # requires Homebrew zlib
    LIBS += /usr/local/opt/zlib/lib/libz.dylib
}
