#ifndef CONTACTEXPORTER_GLOBAL_H
#define CONTACTEXPORTER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CONTACTEXPORTER_LIBRARY)
#  define CONTACTEXPORTERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define CONTACTEXPORTERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CONTACTEXPORTER_GLOBAL_H
