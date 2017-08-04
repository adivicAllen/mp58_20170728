#-------------------------------------------------
#
# Project created by QtCreator 2017-08-04T11:30:57
#
#-------------------------------------------------

QT       -= gui

TARGET = MP5800_API
TEMPLATE = lib

DEFINES += MP5800_API_LIBRARY  \
           MP5800_API_EXPORTS

INCLUDEPATH += /home/allen/mp58_20170728/chroma_api_v2    \
               /home/allen/mp58_20170728/mp5k_v2          \
               /home/allen/mp58_20170728/MP5800_API

SOURCES += mp5800_api.cpp \
    ../MP5800_API.cpp

HEADERS += mp5800_api.h\
        mp5800_api_global.h \
    ../MP5800_API.h \
    ../MP5800_API_const.h \
    ../MP5800_Measure_Item.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
