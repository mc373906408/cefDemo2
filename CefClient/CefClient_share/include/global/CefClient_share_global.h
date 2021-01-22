﻿#ifndef CEFCLIENT_SHARE_GLOBAL_H
#define CEFCLIENT_SHARE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CEFCLIENT_SHARE_LIBRARY)
#  define CEFCLIENT_SHARE_EXPORT Q_DECL_EXPORT
#else
#  define CEFCLIENT_SHARE_EXPORT Q_DECL_IMPORT
#endif

#endif // CEFCLIENT_SHARE_GLOBAL_H