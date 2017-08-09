#ifndef PARSING_GLOBAL_H
#define PARSING_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PARSING_LIBRARY)
#  define PARSINGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PARSINGSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PARSING_GLOBAL_H
