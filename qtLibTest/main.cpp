#include <QCoreApplication>
#include "libtest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Libtest Libtest1;
    Libtest1.t1();
    showqt();
    QString ret = parseSetting("/home/allen/mp58_20170728/Demo3650/InitConfigFile.ini",
                  "Server Config Setting","Server_1_IP").toString();
  ret = parseSetting("/home/allen/mp58_20170728/Demo3650/InitConfigFile.ini",
                  "Server Config Setting","Server_1_Port").toString();
    return a.exec();
}

