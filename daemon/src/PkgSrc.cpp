/*
# DracoPKG - Package management service.
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# DracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
*/

#include <QCoreApplication>
#include <QtDBus>
#include <pkgsrc.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("PkgSrc");
    QCoreApplication::setOrganizationDomain("dracolinux.org");
    QCoreApplication::setOrganizationName("DracoLinux");

    PkgSrc pkgsrc;

    if (!QDBusConnection::systemBus().isConnected()) {
        qWarning("Cannot connect to the D-Bus system bus.");
        return 1;
    }

    if (!QDBusConnection::systemBus().registerService("org.dracolinux.PkgSrc")) {
        qWarning() << QDBusConnection::systemBus().lastError().message();
        return 1;
    }

    QDBusConnection::systemBus().registerObject("/pkgsrc", &pkgsrc, QDBusConnection::ExportAllContents);

    return a.exec();
}
