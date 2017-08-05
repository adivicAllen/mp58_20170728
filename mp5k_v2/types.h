#ifndef TYPES_H
#define TYPES_H
#include <QtGlobal>
#include <unistd.h>
#define sprintf_s sprintf   // allenm  201707234
#define strcpy_s  strcpy
#define sscanf_s  sscanf
typedef void *          HANDLE;
typedef unsigned long   DWORD;
//typedef unsigned int   DWORD;
typedef char            BYTE;
typedef char            byte;

typedef unsigned int    UINT;
typedef int             INT ;


typedef unsigned short   USHORT;
typedef unsigned long long       UINT64;
typedef  long long       INT64;

void Sleep(int sleepTime);

#endif // TYPES_H

