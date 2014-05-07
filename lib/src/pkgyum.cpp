/*
# libDracoPKG - Package management library.
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
*/

#include "pkgyum.h"

PkgYum::PkgYum(QObject *parent) :
    QObject(parent)
{
    checkUpdates = new QProcess(this);
    checkUpdates->setProcessChannelMode(QProcess::MergedChannels);
    connect(checkUpdates,SIGNAL(finished(int)),this,SLOT(checkUpdatesDone()));

    cleanUpdates = new QProcess(this);
    cleanUpdates->setProcessChannelMode(QProcess::MergedChannels);
    connect(cleanUpdates,SIGNAL(finished(int)),this,SLOT(cleanUpdatesDone(int)));

    installUpdates = new QProcess(this);
    installUpdates->setProcessChannelMode(QProcess::MergedChannels);
    connect(installUpdates,SIGNAL(finished(int)),this,SLOT(installUpdatesDone(int)));
    connect(installUpdates,SIGNAL(readyRead()),this,SLOT(installUpdatesProgress()));
}

void PkgYum::checkUpdatesRequest()
{
    QStringList args;
    args << "check-update";
    checkUpdates->start("yum", args);
}

void PkgYum::checkUpdatesDone()
{
    QString result = checkUpdates->readAll();
    QString text;
    checkUpdates->close();
    int updates = 0;

    QStringList pkgs = result.split("\n",QString::SkipEmptyParts);
    for (int i = 0; i < pkgs.size(); ++i) {
        QString line = pkgs.at(i);
        if (!line.contains("Loaded plugins")&&!line.contains("Loading mirror")) {
            text.append(line);
            updates++;
        }
    }

    emit checkUpdatesResult(updates);
    emit checkUpdatesResultText(text);
}

void PkgYum::installUpdatesRequest()
{
    QStringList args;
    args << "-y" << "update";
    installUpdates->start("yum", args);
}

void PkgYum::installUpdatesDone(int status)
{
    installUpdates->close();
    emit installUpdatesFinished(status);
}

bool PkgYum::installUpdatesActive()
{
    if (installUpdates->isOpen()) {
        return true;
    }
    else {
        return false;
    }
}

void PkgYum::installUpdatesProgress()
{
    QString line = installUpdates->readAll();
    emit installUpdatesStatus(line);
}

void PkgYum::cleanUpdatesRequest()
{
    QStringList args;
    args << "clean" << "all";
    cleanUpdates->start("yum", args);
}

void PkgYum::cleanUpdatesDone(int status)
{
    cleanUpdates->close();
    emit cleanUpdatesFinished(status);
}

bool PkgYum::cleanUpdatesActive()
{
    if (cleanUpdates->isOpen()) {
        return true;
    }
    else {
        return false;
    }
}
