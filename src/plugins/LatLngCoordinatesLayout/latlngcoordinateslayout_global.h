#ifndef LATLNGCOORDINATESLAYOUT_GLOBAL_H
#define LATLNGCOORDINATESLAYOUT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LATLNGCOORDINATESLAYOUT_LIBRARY)
#  define LATLNGCOORDINATESLAYOUTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LATLNGCOORDINATESLAYOUTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LATLNGCOORDINATESLAYOUT_GLOBAL_H
