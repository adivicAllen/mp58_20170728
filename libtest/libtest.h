#ifndef LIBTEST_H
#define LIBTEST_H

#include "libtest_global.h"

class LIBTESTSHARED_EXPORT Libtest
{

public:
    Libtest();
    void t1();

};



//#define LIBTESTSHARED_EXPORT Q_DECL_EXPORT


extern "C" {
LIBTESTSHARED_EXPORT void showqt(); //showqt is a function
}

#endif // LIBTEST_H
