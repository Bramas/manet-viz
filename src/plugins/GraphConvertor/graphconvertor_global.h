#ifndef GRAPHCONVERTOR_GLOBAL_H
#define GRAPHCONVERTOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GRAPHCONVERTOR_LIBRARY)
#  define GRAPHCONVERTORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GRAPHCONVERTORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GRAPHCONVERTOR_GLOBAL_H
