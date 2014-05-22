/*
PKGTray - System Tray for DracoPKG
http://www.dracolinux.org

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

PKGTray is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License version 2.
*/

#include "pkgtray.h"

PKGTray::PKGTray(QWidget *parent)
    : QWidget(parent)
{
    busy = false;
    sysupTrayIcon = 0;
    QTimer::singleShot(30000,this,SLOT(cron()));
}

PKGTray::~PKGTray()
{
 
}

void PKGTray::cron()
{
    if (!sysupTrayIcon && !busy) {
        if (!pkgcon())
            QTimer::singleShot(1000,this,SLOT(pkgcon()));
    }
    QTimer::singleShot(7200000,this,SLOT(cron()));
}

bool PKGTray::pkgcon()
{
    bool status = false;
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected()) {
        QDBusInterface iface(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, bus);
        if (iface.isValid() && !updatesActive()) {
		qWarning("OK");
            status = true;
            bus.connect(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,"installUpdatesStatus",this,SLOT(logUpdates(QString)));
            bus.connect(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,"installUpdatesFinished",this,SLOT(updatesDone(int)));
            bus.connect(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,"checkUpdatesResult",this,SLOT(updatesAvail(int)));
            iface.call("checkUpdatesRequest");
        }
    }
    return status;
}

bool PKGTray::updatesActive()
{
    bool status = false;
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected()) {
        QDBusInterface iface(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, bus);
        if (iface.isValid()) {
            QDBusReply<bool> reply = iface.call("installUpdatesActive");
            if (reply)
                status = true;
        }
    }
    return status;
}

void PKGTray::logUpdates(QString data)
{
    log.append(data);
}

void PKGTray::updatesDone(int status)
{
    QMessageBox msgBox;
    msgBox.setDetailedText(log);
    log.clear();

    if (status==0) {
        msgBox.setWindowTitle("Software updates complete");
        QPixmap pix(":/res/installer.png");
        msgBox.setIconPixmap(pix);
        msgBox.setText("<h3>Software update complete</h3>");
        msgBox.setInformativeText("<p>Software updates complete. Some updates may need a logout and/or reboot to apply.</p>");
    }
    else {
        msgBox.setWindowTitle("Software updates failed");
        QPixmap pix(":/res/fail2.png");
        msgBox.setIconPixmap(pix);
        msgBox.setText("<h3>Software updates failed</3>");
        msgBox.setInformativeText("<p>Software updates failed, see details for a complete log.</p>");
    }

    QSpacerItem* horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout* layout = (QGridLayout*)msgBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.exec();

    busy = false;
}

void PKGTray::updatesAvail(int updates)
{
    if (updates>0 && !updatesActive() && !sysupTrayIcon && !busy) {
        if (QSystemTrayIcon::isSystemTrayAvailable()) {
            sysupTrayIcon = new QSystemTrayIcon();
            sysupTrayIcon->setIcon(QIcon(":/res/updates.png"));
            sysupTrayIcon->setToolTip(tr("Software updates available"));
            sysupTrayIcon->show();

            connect(sysupTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(sysupTrayActivated()));
        }
    }
}

void PKGTray::sysupTrayActivated()
{
    if (!updatesActive() && !busy) {
        QMessageBox msgBox;
        msgBox.setText("<h3>Install software updates?</h3>");
        QPixmap pix(":/res/updates2.png");
        msgBox.setIconPixmap(pix);
        msgBox.setWindowTitle("Software updates are available");
        msgBox.setInformativeText("<p>Found software updates. Do you want to install them? They keep your system safe and stuff ...</p>");
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        QSpacerItem* horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)msgBox.layout();
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        int ret = msgBox.exec();

        bool update = false;
        switch (ret) {
        case QMessageBox::Yes:
            update = true;
            break;
        }

        delete sysupTrayIcon;
        sysupTrayIcon = 0;

        if (update) {
            QDBusConnection bus = QDBusConnection::systemBus();
            if (bus.isConnected()) {
                QDBusInterface iface(DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE, bus);
                if (iface.isValid()) {
                    busy = true;
                    iface.call("installUpdatesRequest");
                }
            }
        }
    }
}
