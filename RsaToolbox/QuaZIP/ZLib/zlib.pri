

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD
win32 {
    LIBS += -l$$PWD/zdll
    LIBS += -L$$PWD
}
macx {
    # requires Homebrew zlib
    LIBS += /usr/local/opt/zlib/lib/libz.dylib
}
