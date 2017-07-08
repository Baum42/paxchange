#ifndef LIBpaxchange_GLOBAL_H
#define LIBpaxchange_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PAXCHANGE_LIBRARY)
#  define LIBPAXCHANGE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBPAXCHANGE_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBpaxchange_GLOBAL_H
