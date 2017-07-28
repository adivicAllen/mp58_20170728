#include "libtest.h"
#include <iostream>


Libtest::Libtest()
{
     std::cout << "test show lib test dll" << std::endl;
}

void showqt()
{
     std::cout<< "test show qt" << std::endl;
}

void Libtest::t1()
{
     std::cout<< "t1 :: test show qt" << std::endl;
}
