#
# libDracoPKG - Package management library.
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
#

TARGET                           = DracoPKG
VERSION                          = 1.0.0
TEMPLATE                         = lib
DEFINES                         += DRACOPKG_LIBRARY
QT                              -= gui
QT                              += network
CONFIG                          += release
CONFIG                          -= debug

HEADERS                         += src/DracoPKG_global.h src/dracopkg.h
SOURCES                         += src/dracopkg.cpp

HEADERS                         += src/pkgsrc.h
SOURCES                         += src/pkgsrc.cpp

HEADERS                         += src/pkgyum.h
SOURCES                         += src/pkgyum.cpp

DESTDIR                          = build
OBJECTS_DIR                      = $${DESTDIR}/.obj
MOC_DIR                          = $${DESTDIR}/.moc

isEmpty(PREFIX) {
    PREFIX                       = /usr/local
}

isEmpty(INCLUDEDIR) {
    INCLUDEDIR                   = $${PREFIX}/include
}

isEmpty(DOCDIR) {
    DOCDIR                       = $${PREFIX}/share/doc
}

target.path                      = $${PREFIX}/lib$${LIBSUFFIX}
target_docs.path                 = $${DOCDIR}/$${TEMPLATE}$${TARGET}-$${VERSION}
target_docs.files                = doc/README doc/COPYING.LIB
target_include.path              = $${INCLUDEDIR}/$${TARGET}
target_include.files             = $${HEADERS}

INSTALLS                        += target target_docs target_include
QMAKE_CLEAN                     += $${DESTDIR}/*

!CONFIG(staticlib) {
    QMAKE_POST_LINK             += strip -s $${DESTDIR}/$${TEMPLATE}$${TARGET}.so.$${VERSION}
}

message("PREFIX: $$PREFIX")
message("INCLUDEDIR: $${INCLUDEDIR}")
message("LIBSUFFIX: $${LIBSUFFIX}")
message("DOCDIR: $${DOCDIR}")
