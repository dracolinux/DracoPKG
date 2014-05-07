TEMPLATE                             = subdirs
SUBDIRS                              = lib daemon clients examples
CONFIG                              += ordered
QMAKE_CLEAN                         += Makefile
OTHER_FILES                         += README.md
include(DracoPKG.pri)
