# libDracoPKG
# http://www.dracolinux.org - http://github.com/olear/DracoPKG
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
#

QT -= gui
QT +=network
TARGET = DracoPKG
VERSION = 1.0.0
TEMPLATE = lib
DEFINES += DRACOPKG_LIBRARY

SOURCES += src/dracopkg.cpp src/pkgsrc.cpp src/pkgyum.cpp
HEADERS += src/dracopkg.h src/DracoPKG_global.h src/pkgsrc.h src/pkgyum.h

DESTDIR = ../build/lib

OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc

QMAKE_POST_LINK += $$quote(mkdir -p ../build/include/DracoPKG$$escape_expand(\\n\\t))
for(FILE,HEADERS) {
  QMAKE_POST_LINK += $$quote(cp $${FILE} ../build/include/DracoPKG/$$escape_expand(\\n\\t))
}
