TEMPLATE                             = subdirs
#SUBDIRS                              = userpkg pkgtray
QMAKE_CLEAN                         += Makefile
include(../DracoPKG.pri)

CONFIG(libpkgsrc) {
SUBDIRS=userpkg
}

!CONFIG(libpkgsrc) {
SUBDIRS=userpkg pkgtray
}

CONFIG(userpkg) {
SUBDIRS=userpkg
}
