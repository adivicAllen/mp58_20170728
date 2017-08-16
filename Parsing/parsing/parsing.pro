#-------------------------------------------------
#
# Project created by QtCreator 2017-08-07T11:04:49
#
#-------------------------------------------------

QT       -= gui

TARGET = parsing
TEMPLATE = lib

DEFINES += PARSING_LIBRARY

SOURCES += parsing.cpp

HEADERS += parsing.h\
        parsing_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
