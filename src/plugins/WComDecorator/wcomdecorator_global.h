#ifndef WCOMDECORATOR_GLOBAL_H
#define WCOMDECORATOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WCOMDECORATOR_LIBRARY)
#  define WCOMDECORATORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define WCOMDECORATORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // WCOMDECORATOR_GLOBAL_H