QT                                  += core gui network
TARGET                               = userpkg
TEMPLATE                             = app
#VERSION                              = 20140508
SOURCES                             += src/main.cpp src/mainwindow.cpp
HEADERS                             += src/mainwindow.h
FORMS                               += src/mainwindow.ui
RESOURCES                           += res/files.qrc

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

INCLUDEPATH                         += "../../lib/src"
LIBS                                += -L"$${PREFIX}/lib$${LIBSUFFIX}"
LIBS                                += -L"../../lib/build"
#LIBS                                += "-lDracoPKG"

CONFIG(libpkgsrc) {
LIBS                                += "-lPkgSrc"
}

!CONFIG(libpkgsrc) {
LIBS                                += "-lDracoPKG"
}


#DEFINES += APPV=\"\\\"$${VERSION}\\\"\"
