/*
# libDracoPKG - Package management library.
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
*/

#ifndef PKGYUM_H
#define PKGYUM_H

#include <QObject>
#include <QProcess>
#include <QDebug>

class PkgYum : public QObject
{
    Q_OBJECT
public:
    explicit PkgYum(QObject *parent = 0);
    
signals:
    // Available updates
    void checkUpdatesResult(int result);
    void checkUpdatesResultText(QString data);
    // Install updates
    void installUpdatesStatus(QString data);
    void installUpdatesFinished(int status);
    // Clean updates
    void cleanUpdatesFinished(int status);
public slots:
    // Available updates
    void checkUpdatesRequest();
    // Install updates
    void installUpdatesRequest();
    bool installUpdatesActive();
    // Clean updates
    void cleanUpdatesRequest();
    bool cleanUpdatesActive();
private slots:
    // Available updates
    void checkUpdatesDone();
    // Install updates
    void installUpdatesDone(int status);
    void installUpdatesProgress();
    // Clean updates
    void cleanUpdatesDone(int status);
private:
    // Available updates
    QProcess *checkUpdates;
    // Install updates
    QProcess *installUpdates;
    // Clean updates
    QProcess *cleanUpdates;
};

#endif // PKGYUM_H
