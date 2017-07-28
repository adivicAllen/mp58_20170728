#ifndef MP5000_GLOBAL_H
#define MP5000_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MP5000_LIBRARY)
#  define MP5000SHARED_EXPORT Q_DECL_EXPORT
#else
#  define MP5000SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MP5000_GLOBAL_H
