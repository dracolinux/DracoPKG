TEMPLATE                             = subdirs
#SUBDIRS                              = pkgsrc-bootstrap
QMAKE_CLEAN                         += Makefile
include(../DracoPKG.pri)

!CONFIG(libpkgsrc) {
SUBDIRS= pkgsrc-bootstrap
}

