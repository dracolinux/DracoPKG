/*
PKGTray - System Tray for DracoPKG
http://www.dracolinux.org

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

PKGTray is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
*/

#include <QtGui/QApplication>
#include "pkgtray.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    QApplication::setApplicationName("PKGTray");
    QApplication::setApplicationVersion(APPV);
    QApplication::setOrganizationName("DracoLinux");
    QApplication::setOrganizationDomain("dracolinux.org");
    PKGTray app;

    return a.exec();
}
