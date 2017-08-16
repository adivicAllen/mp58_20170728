TEMPLATE = app
TARGET = heyTester
DESTDIR = $$_PRO_FILE_PWD_/tester
INCLUDEPATH +=/home/allen/mp58_20170728/nonQtDll


SOURCES += \
    tester.cpp
LIBS +=/home/allen/mp58_20170728/nonQtDll/lib/libhey.so

QMAKE_LFLAGS += -Wl,-rpath,"/home/allen/mp58_20170728/nonQtDll/lib"
