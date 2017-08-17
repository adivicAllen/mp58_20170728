QT += core
QT -= gui

TARGET = demo
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../Demo3650.cpp

INCLUDEPATH +=/home/adivic/mp58_20170728/Demo3650    \
               /home/adivic/mp58_20170728/chroma_api_v2    \
               /home/adivic/mp58_20170728/mp5k_v2          \
               /home/adivic/mp58_20170728/MP5800_API

LIBS +=  /home/adivic/mp58_20170728/lib/libMP5800_API.so  \
         /home/adivic/mp58_20170728/lib/libchroma_API_v2.so \
         /home/adivic/mp58_20170728/lib/liblibtest.so  \
         /home/adivic/mp58_20170728/lib/libmp5k_v2.so  \
         /home/adivic/boost_1_61_0/stage/lib/libboost_log.a \
         /home/adivic/boost_1_61_0/stage/lib/libboost_system.a \
         /home/adivic/boost_1_61_0/stage/lib/libboost_thread.a \
         /home/adivic/boost_1_61_0/stage/lib/libboost_filesystem.a


QMAKE_LFLAGS += -Wl,-rpath,"/home/adivic/mp58_20170728/lib"

DESTDIR = $$_PRO_FILE_PWD_/../bin
