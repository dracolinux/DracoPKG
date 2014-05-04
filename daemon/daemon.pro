#
# DracoPKG
# http://www.dracolinux.org - http://github.com/dracolinux
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# DracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
#

QT              += core dbus network xml
QT              -= gui
TARGET           = DracoPKG
CONFIG          += console
CONFIG          -= app_bundle
TEMPLATE         = app
SOURCES         += src/daemon.cpp

DESTDIR          = ../build/sbin

OBJECTS_DIR      = build/obj
MOC_DIR          = build/moc

LIBS            += "-lDracoPKG"

INCLUDEPATH     += "$$_PRO_FILE_PWD_/../lib/src"
LIBS            += -L"$$_PRO_FILE_PWD_/../build/lib"

isEmpty(PREFIX) {
    PREFIX       = /usr/local
}

isEmpty(SBINDIR) {
    SBINDIR      = $$PREFIX/sbin
}

QMAKE_CLEAN	+= ../build/sbin/DracoPKG \
                ../build/etc/dbus-1/system.d/org.dracolinux.DracoPKG.conf \
                ../build/share/dbus-1/system-services/org.dracolinux.DracoPKG.service

QMAKE_POST_LINK += $$quote(mkdir -p ../build/etc/dbus-1/system.d$$escape_expand(\\n\\t))
QMAKE_POST_LINK += $$quote(mkdir -p ../build/share/dbus-1/system-services$$escape_expand(\\n\\t))
QMAKE_POST_LINK += $$quote(cat res/org.dracolinux.DracoPKG.conf > ../build/etc/dbus-1/system.d/org.dracolinux.DracoPKG.conf$$escape_expand(\\n\\t))
QMAKE_POST_LINK += $$quote(cat res/org.dracolinux.DracoPKG.service | sed \'s,_PREFIX_,$$SBINDIR/,\' > ../build/share/dbus-1/system-services/org.dracolinux.DracoPKG.service$$escape_expand(\\n\\t))
