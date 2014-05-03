/*
DracoPKG
http://www.dracolinux.org - http://github.com/dracolinux

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

DracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
*/

#include <QCoreApplication>
#include <QtDBus>
#include <dracopkg.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("DracoPKG");
    QCoreApplication::setOrganizationDomain("dracolinux.org");
    QCoreApplication::setOrganizationName("DracoLinux");

    DracoPKG dracopkg;

    if (getuid()!=0) {
        qWarning("Cannot run as user");
        exit(1);
    }

    if (!QDBusConnection::systemBus().isConnected()) {
        qWarning("Cannot connect to the D-Bus system bus.");
        return 1;
    }

    if (!QDBusConnection::systemBus().registerService("org.dracolinux.DracoPKG")) {
        qWarning() << QDBusConnection::systemBus().lastError().message();
        exit(1);
    }

    //QDBusConnection::systemBus().registerObject("/pkgsrc", &dracopkg.pkgsrc, QDBusConnection::ExportAllContents);
    //QDBusConnection::systemBus().registerObject("/pkgyum", &dracopkg.pkgyum, QDBusConnection::ExportAllContents);
    //QDBusConnection::systemBus().registerObject("/pkgin", &dracopkg.pkgin, QDBusConnection::ExportAllContents);
    //QDBusConnection::systemBus().registerObject("/pkgtools", &dracopkg.pkgtools, QDBusConnection::ExportAllContents);

    return a.exec();
}
