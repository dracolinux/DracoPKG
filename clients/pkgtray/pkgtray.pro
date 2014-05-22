# PKGTray - System Tray for DracoPKG
# http://www.dracolinux.org
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# PKGTray is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
#

QT       += core gui dbus xml
#VERSION = 20140508
TARGET = pkgtray
TEMPLATE = app
SOURCES += src/main.cpp src/pkgtray.cpp
HEADERS  += src/pkgtray.h
RESOURCES += res/res.qrc

DESTDIR = build
OBJECTS_DIR                          = $${DESTDIR}/.obj
MOC_DIR                              = $${DESTDIR}/.moc
RCC_DIR                              = $${DESTDIR}/.qrc
UI_DIR                               = $${DESTDIR}/.ui

include(../../DracoPKG.pri)

target.path                          = $${PREFIX}/bin
target_docs.path                     = $$DOCDIR/$${TARGET}-$${VERSION}
target_docs.files                    = doc/COPYING doc/README

INSTALLS                            += target target_docs
QMAKE_CLEAN                         += -r $${DESTDIR} Makefile
#QMAKE_POST_LINK                     += $$quote(strip -s $${DESTDIR}/$${TARGET}$$escape_expand(\\n\\t))

#DEFINES += APPV=\"\\\"$${VERSION}\\\"\"
