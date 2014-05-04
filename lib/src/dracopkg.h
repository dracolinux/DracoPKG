/*
libDracoPKG
http://www.dracolinux.org - http://github.com/dracolinux

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
*/

#ifndef DRACOPKG_H
#define DRACOPKG_H

#include "DracoPKG_global.h"
#include "pkgsrc.h"
#include "pkgyum.h"

class DRACOPKGSHARED_EXPORT DracoPKG
{
public:
    DracoPKG();
    PkgSrc pkgsrc;
    PkgYum pkgyum;
};

#endif // DRACOPKG_H
