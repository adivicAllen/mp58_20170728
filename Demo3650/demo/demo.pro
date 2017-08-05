QT += core
QT -= gui

TARGET = demo
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../Demo3650.cpp

INCLUDEPATH +=/home/allen/mp58_20170728/Demo3650    \
               /home/allen/mp58_20170728/chroma_api_v2    \
               /home/allen/mp58_20170728/mp5k_v2          \
               /home/allen/mp58_20170728/MP5800_API

LIBS +=  /home/allen/mp58_20170728/lib/libMP5800_API.so  \
         /home/allen/mp58_20170728/lib/libchroma_API_v2.so \
         /home/allen/mp58_20170728/lib/libmp5k_v2.so  \
         /home/allen/boost_1_61_0/stage/lib/libboost_log.a \
         /home/allen/boost_1_61_0/stage/lib/libboost_system.a \
         /home/allen/boost_1_61_0/stage/lib/libboost_thread.a \
         /home/allen/boost_1_61_0/stage/lib/libboost_filesystem.a


QMAKE_LFLAGS += -Wl,-rpath,"/home/allen/mp58_20170728/lib"
