#ifndef JOBLIST_GLOBAL_H
#define JOBLIST_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(JOBLIST_LIBRARY)
#  define JOBLISTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define JOBLISTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // JOBLIST_GLOBAL_H
