libDracoPKG
===========

Package management library.

Requirements
============

 * libQtCore
 * libQtNetwork

Build
=====

```
# qmake
# make
```

Install
=======

```
# qmake
# make install
```

Notes
=====

 * Default installation path is /usr/local, you can override this with the PREFIX variable.
 * Default installation path for the include files are PREFIX/include, you can override this with the INCLUDEDIR variable.
 * Default library suffix is none, you can override this with the LIBSUFFIX variable.
 * Default documentation path is PREFIX/share/doc, you can override this with the DOCDIR variable.
 * INSTALL_ROOT is supported during install.
 * You can build a static library with the variable CONFIG+=staticlib.

libDracoPKG is modular and you can build only parts of the library. Currently we support building the pkgsrc features as a standalone library called libPkgSrc.

```
# qmake CONFIG+=libpkgsrc
```

Usage
=====

See DracoPKG, UserPKG and other applications that uses this library.

Status
======

Usable, but under development.

Links
=====

 * http://github.com/dracolinux/libDracoPKG (REL)
 * http://github.com/olear/libDracoPKG (WIP)
 * http://github.com/olear/libDracoPKG/wiki (TODO)

License
=======

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.

libDracoPKG is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

You should have received a copy of the GNU Lesser General Public License version 2.1 along with libDracoPKG.  If not, see <http://www.gnu.org/licenses/>.
