#
# libDracoPKG - Package management library.
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
#

CONFIG(libpkgsrc) {
    TARGET                       = PkgSrc
    DEFINES                     += PKGSRC_LIBRARY
    HEADERS                     += src/PkgSrc_global.h src/pkgsrc.h
    SOURCES                     += src/pkgsrc.cpp
}

!CONFIG(libpkgsrc) {
    TARGET                       = DracoPKG
    DEFINES                     += DRACOPKG_LIBRARY
    HEADERS                     += src/DracoPKG_global.h src/dracopkg.h
    SOURCES                     += src/dracopkg.cpp
    HEADERS                     += src/pkgsrc.h
    SOURCES                     += src/pkgsrc.cpp
    HEADERS                     += src/pkgyum.h
    SOURCES                     += src/pkgyum.cpp
}

VERSION                          = 1.0.0
TEMPLATE                         = lib
QT                              -= gui
QT                              += network

DESTDIR                          = build
OBJECTS_DIR                      = $${DESTDIR}/.obj
MOC_DIR                          = $${DESTDIR}/.moc

include(../DracoPKG.pri)

target.path                      = $${PREFIX}/lib$${LIBSUFFIX}
target_docs.path                 = $${DOCDIR}/$${TEMPLATE}$${TARGET}-$${VERSION}
target_docs.files                = doc/README doc/COPYING.LIB
target_include.path              = $${INCLUDEDIR}/$${TARGET}
target_include.files             = $${HEADERS}

INSTALLS                        += target target_docs target_include
QMAKE_CLEAN                     += -r $${DESTDIR} Makefile

#!CONFIG(staticlib) {
#    QMAKE_POST_LINK             += strip -s $${DESTDIR}/$${TEMPLATE}$${TARGET}.so.$${VERSION}
#}

INCLUDEPATH                     += "$${PREFIX}/include"
LIBS                            += -L"$${PREFIX}/lib$${LIBSUFFIX}"

