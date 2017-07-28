#include <QCoreApplication>
#include "libtest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Libtest Libtest1;
    Libtest1.t1();
    showqt();
    return a.exec();
}

