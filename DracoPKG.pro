#
# DracoPKG
# http://www.dracolinux.org - http://github.com/dracolinux
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
# DracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
#

TEMPLATE                     = subdirs
SUBDIRS                      = lib daemon clients
CONFIG                      += ordered
VERSION                      = 0.1
DESTDIR                      = build

isEmpty(PREFIX) {
    PREFIX                   = /usr/local
}

isEmpty(SYSCONFDIR) {
    SYSCONFDIR               = /etc/dbus-1/system.d
}

isEmpty(BINDIR) {
    BINDIR                   = $$PREFIX/bin
}

isEmpty(SBINDIR) {
    SBINDIR                  = $$PREFIX/sbin
}

isEmpty(INCLUDEDIR) {
    INCLUDEDIR               = $$PREFIX/include
}

isEmpty(DATAROOTDIR) {
    DATAROOTDIR              = /usr/share/dbus-1/system-services
}

isEmpty(DOCDIR) {
    DOCDIR                   = $$PREFIX/share/doc/DracoPKG-$$VERSION
}

LIBDIR                       = $$PREFIX/lib$$LIBSUFFIX

SYSCONFDIR_FILES             = build/etc/dbus-1/system.d/org.dracolinux.DracoPKG.conf
BINDIR_FILES                 = build/bin/DracoPKG-gui-test
SBINDIR_FILES                = build/sbin/DracoPKG
INCLUDEDIR_FILES             = build/include/DracoPKG
DATAROOTDIR_FILES            = build/share/dbus-1/system-services/org.dracolinux.DracoPKG.service
LIBDIR_FILES                 = build/lib/libDracoPKG.*
DOCDIR_FILES                 = README COPYING COPYING.LIB

target_sysconfdir.path       = $$SYSCONFDIR
target_sysconfdir.files      = $$SYSCONFDIR_FILES

target_bindir.path           = $$BINDIR
target_bindir.files          = $$BINDIR_FILES

target_sbindir.path          = $$SBINDIR
target_sbindir.files         = $$SBINDIR_FILES

target_includedir.path       = $$INCLUDEDIR
target_includedir.files      = $$INCLUDEDIR_FILES

target_datarootdir.path      = $$DATAROOTDIR
target_datarootdir.files     = $$DATAROOTDIR_FILES

target_libdir.path           = $$LIBDIR
target_libdir.files          = $$LIBDIR_FILES

target_docdir.path           = $$DOCDIR
target_docdir.files          = $$DOCDIR_FILES

INSTALLS                    += target_sysconfdir \
                               target_bindir \
                               target_sbindir \
                               target_includedir \
                               target_datarootdir \
                               target_libdir \
                               target_docdir

message("PREFIX: $$PREFIX")
message("SYSCONFDIR: $$SYSCONFDIR")
message("BINDIR: $$BINDIR")
message("SBINDIR: $$SBINDIR")
message("INCLUDEDIR: $$INCLUDEDIR")
message("DATAROOTDIR: $$DATAROOTDIR")
message("DOCDIR: $$DOCDIR")
message("LIBDIR: $$LIBDIR")
message("LIBSUFFIX: $$LIBSUFFIX")
