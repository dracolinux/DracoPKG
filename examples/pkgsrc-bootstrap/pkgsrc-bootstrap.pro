#-------------------------------------------------
#
# Project created by QtCreator 2014-05-07T01:47:08
#
#-------------------------------------------------

QT       += core network
QT       -= gui
TARGET = pkgsrc-bootstrap
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
SOURCES += main.cpp

DESTDIR                              = build
OBJECTS_DIR                          = $${DESTDIR}/.obj

include(../../DracoPKG.pri)

QMAKE_CLEAN                         += -r $${DESTDIR} Makefile

INCLUDEPATH                         += "../../lib/src"
LIBS                                += -L"$${PREFIX}/lib$${LIBSUFFIX}"
LIBS                                += -L"../../lib/build"
LIBS                                += "-lDracoPKG"
