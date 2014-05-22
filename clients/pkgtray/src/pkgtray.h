/*
PKGTray - System Tray for DracoPKG
http://www.dracolinux.org

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

PKGTray is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
*/

#ifndef PKGTRAY_H
#define PKGTRAY_H

#include <QtGui/QWidget>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QPixmap>
#include <QtDBus>
#include <QTimer>
#include <QMessageBox>
#include <QGridLayout>
#include <QSpacerItem>

#define DBUS_SERVICE "org.dracolinux.DracoPKG"
#define DBUS_PATH "/pkgyum"
#define DBUS_INTERFACE "org.dracolinux.DracoPKG.PkgYum"

class PKGTray : public QWidget
{
    Q_OBJECT

public:
    PKGTray(QWidget *parent = 0);
    ~PKGTray();

private slots:
    void cron();
    bool pkgcon();
    bool updatesActive();
    void logUpdates(QString data);
    void updatesDone(int status);
    void updatesAvail(int updates);
    void sysupTrayActivated();

private:
    QSystemTrayIcon *sysupTrayIcon;
    QString log;
    bool busy;
};

#endif // PKGTray_H
