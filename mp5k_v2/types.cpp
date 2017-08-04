#include "types.h"
void Sleep(int sleepTime)
{
    usleep(((useconds_t)sleepTime)*1000);
}
