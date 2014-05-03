/*
libDracoPKG
http://www.dracolinux.org - http://github.com/olear/DracoPKG

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
*/

#ifndef PKGSRC_GLOBAL_H
#define PKGSRC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PKGSRC_LIBRARY)
#  define PKGSRCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PKGSRCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PKGSRC_GLOBAL_H
