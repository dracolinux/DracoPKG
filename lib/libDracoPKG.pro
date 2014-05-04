# libDracoPKG
# http://www.dracolinux.org - http://github.com/dracolinux
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
#

QT                          -= gui
QT                          += network
TARGET                       = DracoPKG
VERSION                      = 1.0.0
TEMPLATE                     = lib
DEFINES                     += DRACOPKG_LIBRARY

SOURCES                     += src/dracopkg.cpp src/pkgsrc.cpp src/pkgyum.cpp
HEADERS                     += src/dracopkg.h src/DracoPKG_global.h src/pkgsrc.h src/pkgyum.h

DESTDIR                      = build
OBJECTS_DIR                  = $${DESTDIR}/.obj
MOC_DIR                      = $${DESTDIR}/.moc

isEmpty(PREFIX) {
    PREFIX                   = /usr/local
}

isEmpty(INCLUDEDIR) {
    INCLUDEDIR               = $${PREFIX}/include/DracoPKG
}

isEmpty(LIBDOCDIR) {
    LIBDOCDIR                   = $$PREFIX/share/doc/$${TEMPLATE}$${TARGET}-$$VERSION
}

target_docs.path             = $$LIBDOCDIR
target_docs.files            = doc/README doc/COPYING.LIB doc/TODO

target.path                  = $${PREFIX}/lib$${LIBSUFFIX}
target_include.path          = $${INCLUDEDIR}
target_include.files         = $${HEADERS}

INSTALLS                    += target target_docs target_include
QMAKE_CLEAN                 += $${DESTDIR}/*
QMAKE_POST_LINK+=strip -s $${DESTDIR}/*.so.*

message("PREFIX: $$PREFIX")
message("INCLUDEDIR: $${INCLUDEDIR}")
message("LIBSUFFIX: $${LIBSUFFIX}")
message("DOCDIR: $${DOCDIR}")
