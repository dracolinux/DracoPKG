#
# DracoPKG - Package management service.
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
SOURCES                             += src/$${TARGET}.cpp

DESTDIR                              = build
OBJECTS_DIR                          = $${DESTDIR}/.obj



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
    DOCDIR                           = $$PREFIX/share/doc
}

isEmpty(ADMIN_GROUP) {
    ADMIN_GROUP                      = wheel
}

target.path                          = $${PREFIX}/sbin
target_dbus_conf.path                = $${DBUS_CONF}
target_dbus_service.path             = $${DBUS_SERVICE}

target_dbus_conf.commands            = $$quote(cat res/org.dracolinux.conf | sed "\"s/_SERVICE_/$${TARGET}/g;s/_GROUP_/$${ADMIN_GROUP}/"\" > ${INSTALL_ROOT}$${DBUS_CONF}/org.dracolinux.$${TARGET}.conf$$escape_expand(\\n\\t))

target_dbus_service.commands         = $$quote(echo "\"[D-BUS Service]"\" > ${INSTALL_ROOT}$${DBUS_SERVICE}/org.dracolinux.$${TARGET}.service$$escape_expand(\\n\\t))
target_dbus_service.commands        += $$quote(echo "\"Name=org.dracolinux.$${TARGET}"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/org.dracolinux.$${TARGET}.service$$escape_expand(\\n\\t))
target_dbus_service.commands        += $$quote(echo "\"Exec=$${PREFIX}/sbin/$${TARGET}"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/org.dracolinux.$${TARGET}.service$$escape_expand(\\n\\t))
target_dbus_service.commands        += $$quote(echo "\"User=root"\" >> ${INSTALL_ROOT}$${DBUS_SERVICE}/org.dracolinux.$${TARGET}.service$$escape_expand(\\n\\t))

target_docs.path                     = $$DOCDIR/$${TARGET}-$${VERSION}
target_docs.files                    = doc/COPYING doc/README

INSTALLS                            += target target_dbus_conf target_dbus_service target_docs
QMAKE_CLEAN                         += $${DESTDIR}/*
QMAKE_POST_LINK                     += $$quote(strip -s $${DESTDIR}/$${TARGET}$$escape_expand(\\n\\t))

INCLUDEPATH                         += "$${PREFIX}/include"
LIBS                                += -L"$${PREFIX}/lib"
LIBS                                += "-l$${TARGET}"

OTHER_FILES                         += res/org.dracolinux.conf

message("PREFIX: $$PREFIX")
message("DBUS_CONF: $${DBUS_CONF}")
message("DBUS_SERVICE: $${DBUS_SERVICE}")
message("DOCDIR: $${DOCDIR}")
