/*
libDracoPKG
http://www.dracolinux.org - http://github.com/olear/DracoPKG

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
*/

#ifndef PKGSRC_H
#define PKGSRC_H

#include <QObject>
#include <QFtp>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QCryptographicHash>
#include <QProcess>
#include <QDebug>

class PkgSrc : public QObject
{
    Q_OBJECT
public:
    explicit PkgSrc(QObject *parent = 0);
    
signals:
    // Download pkgsrc
    void downloadStatus(qint64 start, qint64 end);
    void downloadFinished(int status);
    // Extract pkgsrc
    void extractStatus(QString data);
    void extractFinished(int status);
    // Bootstrap pkgsrc
    void bootstrapStatus(QString data);
    void bootstrapFinished(int status);
    // pkgsrc bmake
    void bmakeStatus(QString data);
    void bmakeFinished(int status);
    // package build options
    void packageOptionsResult(QStringList options);
    // package depends
    void packageDependsResult(QStringList depends);
    // package clean build
    void packageCleanFinished(int status);
    // packages installed
    void packagesInstalledResult(QStringList packages);
    // package version
    void packageVersionResult(QString version);
    // package name
    void packageNameResult(QString name);
    // packages vulns
    void packagesVulnsResult(QStringList result);
    
public slots:
    // Download pkgsrc
    bool downloadStart();
    // Extract pkgsrc
    bool extractStart();
    // Bootsrap pkgsrc
    bool bootstrapStart();
    bool bootstrapClean();
    bool bootstrapActive();
    // pkgsrc bmake
    bool bmakeStart(QString pkg,QString cat,QString options, QString action);
    bool bmakeStop();
    bool bmakeActive();
    // pkgsrc branch version
    QString branchVersion();
    // package build options
    bool packageOptionsRequest(QString pkg, QString cat);
    // package depends
    bool packageDependsRequest(QString pkg, QString cat);
    // package category list
    QStringList categoryList();
    // package list
    QStringList packageList(QString cat, QString search);
    // package clean build
    void packageCleanRequest();
    // packages installed
    void packagesInstalledRequest();
    // package version
    bool packageVersionRequest(QString pkg, QString cat);
    // package name
    bool packageNameRequest(QString pkg, QString cat);
    // packacges vulns
    void packagesVulnsRequest();

private slots:
    bool dirClean(QString dirName);
    // Download pkgsrc
    void downloadProgress(qint64 start, qint64 end);
    void downloadDone();
    // Extract pkgsrc
    void extractProgress();
    void extractDone(int status);
    // Bootstrap pkgsrc
    void bootstrapProgress();
    void bootstrapDone(int status);
    // pkgsrc bmake
    void bmakeProgress();
    void bmakeDone(int status);
    // package build options
    void pkgBuildOptionsDone(int status);
    // package depends
    void pkgDependsDone(int status);
    // package clean build
    void pkgCleanDone(int status);
    // packages installed
    void pkgInstalledDone(int status);
    // package version
    void pkgVersionDone(int status);
    // package name
    void pkgNameDone(int status);
    // packages fetch vulns list
    void pkgVulnDownloadDone(int status);
    // packacges vulns check
    void pkgVulnCheckDone(int status);

private:
    // Download pkgsrc
    QFtp *pkgsrcDownload;
    QFile *pkgsrcDownloadFile;
    QFile *pkgsrcDownloadMd5;
    // Extract pkgsrc
    QProcess *pkgsrcExtract;
    // Bootstrap pkgsrc
    QProcess *pkgsrcBootstrap;
    // pkgsrc bmake
    QProcess *pkgsrcBmake;
    // package build options
    QProcess *pkgBuildOptions;
    // package depends
    QProcess *pkgDepends;
    // package clean build
    QProcess *pkgClean;
    // packages installed
    QProcess *pkgInstalled;
    // package version
    QProcess *pkgVersion;
    // package name
    QProcess *pkgName;
    // packages fetch vulns list
    QProcess *pkgVulnDownload;
    // packacges vulns check
    QProcess *pkgVulnCheck;
};

#endif // PKGSRC_H
