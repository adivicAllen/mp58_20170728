#-------------------------------------------------
#
# Project created by QtCreator 2017-08-09T02:47:42
#
#-------------------------------------------------

QT       -= gui

TARGET = sharedLibray
TEMPLATE = lib

DEFINES += SHAREDLIBRAY_LIBRARY

SOURCES += sharedlibray.cpp

HEADERS += sharedlibray.h\
        sharedlibray_global.h
DESTDIR = $$_PRO_FILE_PWD_/../lib

unix {
    target.path = /usr/lib
    INSTALLS += target
}
