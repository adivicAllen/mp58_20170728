#-------------------------------------------------
#
# Project created by QtCreator 2017-07-24T11:05:34
#
#-------------------------------------------------

QT       -= gui

TARGET = libtest
TEMPLATE = lib

DEFINES += LIBTEST_LIBRARY

SOURCES += libtest.cpp

HEADERS += libtest.h\
        libtest_global.h

DESTDIR = $$_PRO_FILE_PWD_/../lib
message($$_PRO_FILE_)
message($$_PRO_FILE_PWD_)

unix {
    target.path = /usr/lib
    INSTALLS += target
}
