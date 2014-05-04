/*
libDracoPKG
http://www.dracolinux.org - http://github.com/olear/DracoPKG

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
*/

#ifndef DRACOPKG_GLOBAL_H
#define DRACOPKG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DRACOPKG_LIBRARY)
#  define DRACOPKGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DRACOPKGSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DRACOPKG_GLOBAL_H
