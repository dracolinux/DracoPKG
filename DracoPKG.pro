#
# DracoPKG - Simple Package management D-Bus service.
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# DracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
#

QT                                  += core dbus network xml
QT                                  -= gui
TARGET                               = DracoPKG
VERSION                              = 1.0.0
CONFIG                              += console
CONFIG                              -= app_bundle
TEMPLATE                             = app
SOURCES                             += DracoPKG.cpp

DESTDIR                              = build
OBJECTS_DIR                          = $${DESTDIR}/.obj

LIBS                                += "-lDracoPKG"

isEmpty(PREFIX) {
    PREFIX                           = /usr/local
}

isEmpty(DBUS_CONF) {
    DBUS_CONF                        = /etc/dbus-1/system.d
}

isEmpty(DBUS_SERVICE) {
    DBUS_SERVICE                     = /usr/share/dbus-1/system-services
}

isEmpty(DOCDIR) {
    DOCDIR                           = $$PREFIX/share/doc/$${TARGET}-$${VERSION}
}

target.path                          = $${PREFIX}/sbin
target_dbus_conf.path                = $${DBUS_CONF}
target_dbus_service.path             = $${DBUS_SERVICE}

target_dbus_conf.files               = org.dracolinux.DracoPKG.conf
target_dbus_service.commands         = $$quote(echo "\"[D-BUS Service]"\" > ${INSTALL_ROOT}$${DBUS_SERVICE}/org.dracolinux.DracoPKG.service$$escape_expand(\\n\\t))
target_dbus_service.commands        += $$quote(echo "\"Name=org.dracolinux.DracoPKG"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/org.dracolinux.DracoPKG.service$$escape_expand(\\n\\t))
target_dbus_service.commands        += $$quote(echo "\"Exec=$${PREFIX}/sbin/DracoPKG"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/org.dracolinux.DracoPKG.service$$escape_expand(\\n\\t))
target_dbus_service.commands        += $$quote(echo "\"User=root"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/org.dracolinux.DracoPKG.service$$escape_expand(\\n\\t))

target_docs.path                     = $$DOCDIR
target_docs.files                    = COPYING README

INSTALLS                            += target target_dbus_conf target_dbus_service target_docs
QMAKE_CLEAN                         += $${DESTDIR}/*
QMAKE_POST_LINK                     += $$quote(strip -s $${DESTDIR}/$${TARGET}$$escape_expand(\\n\\t))

message("PREFIX: $$PREFIX")
message("DBUS_CONF: $${DBUS_CONF}")
message("DBUS_SERVICE: $${DBUS_SERVICE}")
message("DOCDIR: $${DOCDIR}")
