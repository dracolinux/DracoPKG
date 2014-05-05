/*
# DracoPKG - Package management service.
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# DracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
*/

#include <QCoreApplication>
#include <QtDBus>
#include <DracoPKG/dracopkg.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("DracoPKG");
    QCoreApplication::setOrganizationDomain("dracolinux.org");
    QCoreApplication::setOrganizationName("DracoLinux");

    DracoPKG dracopkg;

    if (!QDBusConnection::systemBus().isConnected()) {
        qWarning("Cannot connect to the D-Bus system bus.");
        return 1;
    }

    if (!QDBusConnection::systemBus().registerService("org.dracolinux.DracoPKG")) {
        qWarning() << QDBusConnection::systemBus().lastError().message();
        return 1;
    }

    QDBusConnection::systemBus().registerObject("/pkgsrc", &dracopkg.pkgsrc, QDBusConnection::ExportAllContents);
    QDBusConnection::systemBus().registerObject("/pkgyum", &dracopkg.pkgyum, QDBusConnection::ExportAllContents);
    //QDBusConnection::systemBus().registerObject("/pkginstall", &dracopkg.pkginstall, QDBusConnection::ExportAllContents);
    //QDBusConnection::systemBus().registerObject("/pkgin", &dracopkg.pkgin, QDBusConnection::ExportAllContents);
    //QDBusConnection::systemBus().registerObject("/pkgtools", &dracopkg.pkgtools, QDBusConnection::ExportAllContents);

    return a.exec();
}
