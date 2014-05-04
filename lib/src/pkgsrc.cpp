/*
libDracoPKG
http://www.dracolinux.org - http://github.com/olear/DracoPKG

Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.

libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
*/

#include "pkgsrc.h"

PkgSrc::PkgSrc(QObject *parent) :
    QObject(parent)
{
    pkgsrcDownload = new QFtp(this);
    connect(pkgsrcDownload,SIGNAL(done(bool)),SLOT(downloadDone()));
    connect(pkgsrcDownload,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));

    pkgsrcExtract = new QProcess(this);
    pkgsrcExtract->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgsrcExtract,SIGNAL(finished(int)),this,SLOT(extractDone(int)));
    connect(pkgsrcExtract,SIGNAL(readyRead()),this,SLOT(extractProgress()));

    pkgsrcBootstrap = new QProcess(this);
    pkgsrcBootstrap->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgsrcBootstrap,SIGNAL(finished(int)),this,SLOT(bootstrapDone(int)));
    connect(pkgsrcBootstrap,SIGNAL(readyRead()),this,SLOT(bootstrapProgress()));

    pkgsrcBmake = new QProcess(this);
    pkgsrcBmake->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgsrcBmake,SIGNAL(finished(int)),this,SLOT(bmakeDone(int)));
    connect(pkgsrcBmake,SIGNAL(readyRead()),this,SLOT(bmakeProgress()));

    pkgBuildOptions = new QProcess(this);
    pkgBuildOptions->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgBuildOptions,SIGNAL(finished(int)),this,SLOT(pkgBuildOptionsDone(int)));

    pkgDepends = new QProcess(this);
    pkgDepends->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgDepends,SIGNAL(finished(int)),this,SLOT(pkgDependsDone(int)));

    pkgClean = new QProcess(this);
    pkgClean->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgClean,SIGNAL(finished(int)),this,SLOT(pkgCleanDone(int)));

    pkgInstalled = new QProcess(this);
    pkgInstalled->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgInstalled,SIGNAL(finished(int)),this,SLOT(pkgInstalledDone(int)));

    pkgVersion = new QProcess(this);
    pkgVersion->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgVersion,SIGNAL(finished(int)),this,SLOT(pkgVersionDone(int)));

    pkgName = new QProcess(this);
    pkgName->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgName,SIGNAL(finished(int)),this,SLOT(pkgNameDone(int)));

    pkgVulnDownload = new QProcess(this);
    pkgVulnDownload->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgVulnDownload,SIGNAL(finished(int)),this,SLOT(pkgVulnDownloadDone(int)));

    pkgVulnCheck = new QProcess(this);
    pkgVulnCheck->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgVulnCheck,SIGNAL(finished(int)),this,SLOT(pkgVulnCheckDone(int)));
}

bool PkgSrc::downloadStart()
{
    QString tmpdir;
    int uid = 0;
    uid = getuid();

    if (uid>0) {
        tmpdir = QDir::homePath()+"/userpkg/";
    }
    else {
        tmpdir = "/root/";
    }

    pkgsrcDownloadFile = new QFile(tmpdir+"pkgsrc.tar.xz");
    pkgsrcDownloadMd5 = new QFile(tmpdir+"pkgsrc.tar.xz.MD5");

    if (pkgsrcDownloadMd5->open(QIODevice::WriteOnly) && pkgsrcDownloadFile->open(QIODevice::WriteOnly)) {
        pkgsrcDownload->connectToHost("ftp.netbsd.org");
        pkgsrcDownload->login("ftp","ftp");
        pkgsrcDownload->cd("pub");
        pkgsrcDownload->cd("pkgsrc");
        pkgsrcDownload->cd("stable");
        pkgsrcDownload->get("pkgsrc.tar.xz", pkgsrcDownloadFile);
        pkgsrcDownload->get("pkgsrc.tar.xz.MD5", pkgsrcDownloadMd5);
        pkgsrcDownload->close();
        return true;
    }
    else {
        delete pkgsrcDownloadFile;
        delete pkgsrcDownloadMd5;
        return false;
    }
}
void PkgSrc::downloadDone()
{
    pkgsrcDownloadFile->close();
    pkgsrcDownloadMd5->close();

    QString md5Sum;
    QString fileSum;

    if (pkgsrcDownloadMd5->open(QIODevice::ReadOnly)) {
        QTextStream textStream(pkgsrcDownloadMd5);
        QString stream =textStream.readAll();
        md5Sum= stream.split(" ").takeLast().simplified().replace(" ","");
    }

    pkgsrcDownloadMd5->close();
    delete pkgsrcDownloadMd5;

    if (pkgsrcDownloadFile->open(QIODevice::ReadOnly)) {
        QByteArray fileData = pkgsrcDownloadFile->readAll();
        QByteArray hashData = QCryptographicHash::hash(fileData,QCryptographicHash::Md5);
        fileSum = hashData.toHex();
    }

    pkgsrcDownloadFile->close();
    delete pkgsrcDownloadFile;

    if (fileSum==md5Sum) {
        emit downloadFinished(1);
    }
    else {
        emit downloadFinished(0);
    }
}
void PkgSrc::downloadProgress(qint64 start, qint64 end)
{
    emit downloadStatus(start,end);
}

bool PkgSrc::extractStart()
{
    bool status = false;
    QString pkgsrc_dir;
    QString tmpdir;
    int uid = 0;
    uid = getuid();

    if (uid>0) {
        pkgsrc_dir = QDir::homePath()+"/userpkg/pkgsrc";
        tmpdir = QDir::homePath()+"/userpkg/";
    }
    else {
        pkgsrc_dir = "/usr/pkgsrc";
        tmpdir = "/root/";
    }

    QDir pkgsrc;
    if (!pkgsrc.exists(pkgsrc_dir)&&!pkgsrcBootstrap->isOpen()&&!pkgsrcBmake->isOpen()) {
        QStringList args;
        args << "xvf" << tmpdir+"pkgsrc.tar.xz";
        if (uid>0) {
            pkgsrcExtract->setWorkingDirectory(QDir::homePath()+"/userpkg");
        }
        else {
            pkgsrcExtract->setWorkingDirectory("/usr");
        }
        pkgsrcExtract->start("tar",args);
        pkgsrcExtract->waitForStarted();
        status = true;
    }

    return status;
}

void PkgSrc::extractDone(int status)
{
    pkgsrcExtract->close();
    emit extractFinished(status);
}

void PkgSrc::extractProgress()
{
    emit extractStatus(pkgsrcExtract->readAll());
}

bool PkgSrc::bootstrapStart()
{
    bool status = false;
    QString bmake_exec;
    int uid = 0;
    uid = getuid();

    if (uid>0) {
        bmake_exec = QDir::homePath()+"/userpkg/pkg/bin/bmake";
    }
    else {
        QFile native_bmake("/usr/bin/bmake");
        if (native_bmake.exists()) {
            bmake_exec = "/usr/bin/bmake";
        }
        else {
            bmake_exec = "/usr/pkg/bin/bmake";
        }
    }

    QFile bmake(bmake_exec);
    if (!bmake.exists()&&!pkgsrcBootstrap->isOpen()&&!pkgsrcBmake->isOpen()) {
        QStringList args;

        QProcessEnvironment buildEnv = QProcessEnvironment::systemEnvironment();
        buildEnv.insert("MAKE_JOBS","4");

        QFile bash("/bin/bash");
        if (bash.exists()) {
            buildEnv.insert("SH","/bin/bash");
        }

        if (uid>0) {
            args << "--pkgdbdir="+QDir::homePath()+"/userpkg/var/db/pkg" << "--prefix="+QDir::homePath()+"/userpkg/pkg" << "--varbase="+QDir::homePath()+"/userpkg/var" << "--unprivileged"; // <<  "--prefer-pkgsrc=yes";
            pkgsrcBootstrap->setWorkingDirectory(QDir::homePath()+"/userpkg/pkgsrc/bootstrap");
        }
        else {
            args << "--prefix=/usr/pkg" << "--varbase=/usr/pkg/var"; // << "--prefer-pkgsrc=yes";
            pkgsrcBootstrap->setWorkingDirectory("/usr/pkgsrc/bootstrap");
        }

        pkgsrcBootstrap->setProcessEnvironment(buildEnv);
        pkgsrcBootstrap->start("./bootstrap",args);
        status = true;
    }

    return status;
}

void PkgSrc::bootstrapDone(int status)
{
    pkgsrcBootstrap->close();
    bootstrapClean();
    bmakeStart("pkgclean","pkgtools","","install");

    emit bootstrapFinished(status);
}

void PkgSrc::bootstrapProgress()
{
    emit bootstrapStatus(pkgsrcBootstrap->readAll());
}

bool PkgSrc::bootstrapClean()
{
    QString dir;
    int uid = 0;
    uid = getuid();

    if (uid>0) {
        dir = QDir::homePath()+"/userpkg/pkgsrc/bootstrap/work";
    }
    else {
        dir = "/usr/pkgsrc/bootstrap/work";
    }

    if (dirClean(dir)) {
        return true;
    }
    else {
        return false;
    }
}

bool PkgSrc::bootstrapActive()
{
    if (pkgsrcBootstrap->isOpen()) {
        return true;
    }
    else {
        return false;
    }
}

bool PkgSrc::dirClean(QString dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = dirClean(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }

    return result;
}

bool PkgSrc::bmakeStart(QString pkg, QString cat, QString options, QString action)
{
    bool status=false;
    if (!pkg.isEmpty()&&!cat.isEmpty()&&!action.isEmpty()&&!pkgsrcBmake->isOpen()&&!pkgsrcBootstrap->isOpen()) {
        QStringList args;
        args << action;
        QProcessEnvironment buildEnv = QProcessEnvironment::systemEnvironment();
        buildEnv.insert("MAKE_JOBS","4");

        if (!options.isEmpty()) {
            buildEnv.insert("PKG_OPTIONS."+pkg, options);
        }

        buildEnv.insert("SKIP_LICENSE_CHECK","yes");
        buildEnv.insert("ALLOW_VULNERABLE_PACKAGES","yes");
        pkgsrcBmake->setProcessEnvironment(buildEnv);

        int uid = 0;
        uid = getuid();
        QString bmake_exec;
        QString workdir;

        if (uid>0) {
            workdir = QDir::homePath()+"/userpkg/pkgsrc/"+cat+"/"+pkg;
            bmake_exec = QDir::homePath()+"/userpkg/pkg/bin/bmake";
        }
        else {
            workdir = "/usr/pkgsrc/"+cat+"/"+pkg;
            QFile native_bmake("/usr/bin/bmake");
            if (native_bmake.exists()) {
                bmake_exec = "/usr/bin/bmake";
            }
            else {
                bmake_exec = "/usr/pkg/bin/bmake";
            }
        }

        if (!bmake_exec.isEmpty()&&!workdir.isEmpty()) {
            pkgsrcBmake->setWorkingDirectory(workdir);
            pkgsrcBmake->start(bmake_exec,args);
            status = true;
        }
    }

    return status;
}

void PkgSrc::bmakeDone(int status)
{
    pkgsrcBmake->close();
    emit bmakeFinished(status);
}

void PkgSrc::bmakeProgress()
{
    emit bmakeStatus(pkgsrcBmake->readAll());
}

bool PkgSrc::bmakeStop()
{
    bool status = false;
    if (pkgsrcBmake->isOpen()) {
        pkgsrcBmake->close();
        status = true;
    }

    return status;
}

bool PkgSrc::bmakeActive()
{
    if (pkgsrcBmake->isOpen()) {
        return true;
    }
    else {
        return false;
    }
}

QString PkgSrc::branchVersion()
{
    QString branch;
    QString tagFile;
    QString pkgsrcDir;
    int uid = 0;
    uid = getuid();

    if (uid>0) {
        tagFile = QDir::homePath()+"/userpkg/pkgsrc/CVS/Tag";
        pkgsrcDir = QDir::homePath()+"/userpkg/pkgsrc";
    }
    else {
        tagFile = "/usr/pkgsrc/CVS/Tag";
        pkgsrcDir = "/usr/pkgsrc";
    }

    QFile pkgsrcTagFile(tagFile);
    if (pkgsrcTagFile.exists()) {
        if (pkgsrcTagFile.open(QIODevice::ReadOnly)) {
            QTextStream textStream(&pkgsrcTagFile);
            branch = textStream.readAll().mid(8);
        }
        pkgsrcTagFile.close();
    }
    else {
        QDir pkgsrCurrent;
        if (pkgsrCurrent.exists(pkgsrcDir)) {
            branch= "current";
        }
    }

    return branch.simplified();
}

bool PkgSrc::packageOptionsRequest(QString pkg, QString cat)
{
    bool status = false;
    if (!pkg.isEmpty()&&!cat.isEmpty()) {
        QStringList args;
        args << "show-options";
        int uid = 0;
        uid = getuid();
        QString bmake_exec;
        QString workdir;

        if (uid>0) {
            workdir = QDir::homePath()+"/userpkg/pkgsrc/"+cat+"/"+pkg;
            bmake_exec = QDir::homePath()+"/userpkg/pkg/bin/bmake";
        }
        else {
            workdir = "/usr/pkgsrc/"+cat+"/"+pkg;
            QFile native_bmake("/usr/bin/bmake");
            if (native_bmake.exists()) {
                bmake_exec = "/usr/bin/bmake";
            }
            else {
                bmake_exec = "/usr/pkg/bin/bmake";
            }
        }

        if (!bmake_exec.isEmpty()&&!workdir.isEmpty()) {
            pkgBuildOptions->setWorkingDirectory(workdir);
            pkgBuildOptions->start(bmake_exec,args);
            status = true;
        }
    }

    return status;
}

void PkgSrc::pkgBuildOptionsDone(int status)
{
    QString options;
    options = pkgBuildOptions->readAll();
    pkgBuildOptions->close();

    QStringList output;
    if (status==0) {
        QStringList optionsList = options.split("\n");
        QStringList optionsAvail;
        QStringList optionsSelected;
        QString tmp;
        int optionsAdd = 0;
        int optionsDef = 0;
        for (int i = 0; i < optionsList.size(); ++i) {
            if (optionsList.at(i) == "Any of the following general options may be selected:")
                optionsAdd=1;
            if (optionsList.at(i) == "These options are enabled by default:")
                optionsAdd=0;
            if (optionsList.at(i) == "These options are currently enabled:")
                optionsDef=1;
            if (optionsList.at(i) == "You can select which build options to use by setting PKG_DEFAULT_OPTIONS")
                optionsDef=0;

            if (optionsDef==1 && optionsList.at(i) !="These options are currently enabled:" && optionsList.at(i) != "You can select which build options to use by setting PKG_DEFAULT_OPTIONS") {
                if (!optionsList.at(i).isEmpty()) {
                    tmp.append(optionsList.at(i).simplified().replace(" ","|"));
                }
            }
            if (optionsAdd==1 && optionsList.at(i) != "Any of the following general options may be selected:") {
                QStringList getFlag = optionsList.at(i).split(" ",QString::SkipEmptyParts);
                if(!getFlag.isEmpty()) {
                    QString value = getFlag.takeFirst().simplified().replace(" ","");
                    if (!value.isEmpty()) {
                        optionsAvail << value;
                    }
                }
            }
        }

        QStringList tmpList = tmp.split("|",QString::SkipEmptyParts);
        for (int i = 0; i < tmpList.size(); ++i) {optionsSelected << tmpList.at(i);}
        for (int i = 0; i < optionsAvail.size(); ++i) {
            QString availOpt = optionsAvail.at(i);
            if (!availOpt.isEmpty()) {
                int foundOpt=0;
                for (int i = 0; i < optionsSelected.size(); ++i) {
                    if (availOpt==optionsSelected.at(i)) {
                        foundOpt=1;
                    }
                }
                if (foundOpt==1) {
                    output<<"+"+availOpt;
                }
                else {
                    output<<"-"+availOpt;
                }
            }
        }
    }

    emit packageOptionsResult(output);
}

bool PkgSrc::packageDependsRequest(QString pkg, QString cat)
{
    bool status = false;
    if (!pkg.isEmpty()&&!cat.isEmpty()) {
        QStringList args;
        args << "show-depends";
        int uid = 0;
        uid = getuid();
        QString bmake_exec;
        QString workdir;

        if (uid>0) {
            workdir = QDir::homePath()+"/userpkg/pkgsrc/"+cat+"/"+pkg;
            bmake_exec = QDir::homePath()+"/userpkg/pkg/bin/bmake";
        }
        else {
            workdir = "/usr/pkgsrc/"+cat+"/"+pkg;
            QFile native_bmake("/usr/bin/bmake");
            if (native_bmake.exists()) {
                bmake_exec = "/usr/bin/bmake";
            }
            else {
                bmake_exec = "/usr/pkg/bin/bmake";
            }
        }

        if (!bmake_exec.isEmpty()&&!workdir.isEmpty()) {
            pkgDepends->setWorkingDirectory(workdir);
            pkgDepends->start(bmake_exec,args);
            status = true;
        }
    }

    return status;
}

void PkgSrc::pkgDependsDone(int status)
{
    QString value = pkgDepends->readAll();
    pkgDepends->close();
    QStringList output;
    if (status==0&&!value.isEmpty()) {
        QStringList depends = value.split("\n",QString::SkipEmptyParts);
        for (int i = 0; i < depends.size(); ++i) {
            if (!depends.at(i).isEmpty()) {
                output << depends.at(i);
            }
        }
    }

    emit packageDependsResult(output);
}

QStringList PkgSrc::categoryList()
{
    QStringList output;
    int uid = 0;
    uid = getuid();
    QString workdir;

    if (uid>0) {
        workdir = QDir::homePath()+"/userpkg/pkgsrc/";
    }
    else {
        workdir = "/usr/pkgsrc/";
    }

    QDir pkgsrc(workdir);
    QFileInfoList pkgsrcList(pkgsrc.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs));
    foreach (QFileInfo folder, pkgsrcList) {
        if (folder.isDir()) {
            if (folder.baseName()!= "regress") {
                QFile Makefile;
                if (Makefile.exists(folder.filePath()+"/Makefile"))
                {
                    output << folder.fileName();
                }
            }
        }
    }

    return output;
}

QStringList PkgSrc::packageList(QString cat, QString search)
{
    QStringList output;
    int uid = 0;
    uid = getuid();
    QString workdir;
    QString searchdir;

    if (uid>0) {
        workdir = QDir::homePath()+"/userpkg/pkgsrc/";
        searchdir = workdir;
    }
    else {
        workdir = "/usr/pkgsrc/";
        searchdir = workdir;
    }

    if (!cat.isEmpty()) {
        workdir = workdir+cat;
    }

    if (!search.isEmpty()) {
        QStringList categories = categoryList();
        for (int i = 0; i < categories.size(); ++i) {
            QDir pkgsrc(searchdir+categories.at(i));
            QFileInfoList pkgsrcList(pkgsrc.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs));
            foreach (QFileInfo folder, pkgsrcList) {
                if (folder.isDir()) {
                    QFile Makefile(folder.filePath()+"/Makefile");
                    if (Makefile.exists(folder.filePath()+"/Makefile") && folder.fileName().contains(search)) {
                        output << categories.at(i)+"|"+folder.fileName();
                    }
                }
            }
        }
    }
    else
    {
        QDir pkgsrc(workdir);
        QFileInfoList pkgsrcList(pkgsrc.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs));
        foreach (QFileInfo folder, pkgsrcList) {
            if (folder.isDir()) {
                QFile Makefile(folder.filePath()+"/Makefile");
                if (Makefile.exists(folder.filePath()+"/Makefile")) {
                    if (!cat.isEmpty()) {
                        output << cat+"|"+folder.fileName();
                    }
                    else {
                        output << folder.fileName();
                    }
                }
            }
        }
    }

    return output;
}

void PkgSrc::packageCleanRequest()
{
    int uid = 0;
    uid = getuid();
    QString workdir;

    if (uid>0) {
        workdir = QDir::homePath()+"/userpkg/pkg/bin/";
    }
    else {
        workdir = "/usr/pkg/bin/";
    }

    pkgClean->start(workdir+"pkgclean");
}

void PkgSrc::pkgCleanDone(int status)
{
    emit packageCleanFinished(status);
}

void PkgSrc::packagesInstalledRequest()
{
    int uid = 0;
    uid = getuid();
    QString workdir;

    if (uid>0) {
        workdir = QDir::homePath()+"/userpkg/pkg/sbin/";
    }
    else {
        workdir = "/usr/pkg/sbin/";
    }

    pkgInstalled->start(workdir+"pkg_info");
}

void PkgSrc::pkgInstalledDone(int status)
{
    QStringList output;
    if (status==0) {
        QString packages = pkgInstalled->readAll();
        QStringList pkgList = packages.split("\n",QString::SkipEmptyParts);
        foreach(QString pkg,pkgList) {
            QStringList pkgInfo = pkg.split(" ",QString::SkipEmptyParts);
            if (!pkgInfo.isEmpty()) {
                QString pkgFile = pkgInfo.takeFirst();
                QString pkgDesc;
                if (!pkgInfo.isEmpty()) {
                    foreach(QString pkgInfoDesc,pkgInfo) {
                        pkgDesc.append(pkgInfoDesc+" ");
                    }
                }
                QString pkgVersion = pkgFile.mid(pkgFile.lastIndexOf("-")).replace("-","");
                QString pkgName = pkgFile.left(pkgFile.lastIndexOf("-"));

                if (!pkgName.isEmpty()&&!pkgVersion.isEmpty()&&!pkgDesc.isEmpty()) {
                    output << pkgName+"|"+pkgVersion+"|"+pkgDesc;
                }
            }
        }
    }

    emit packagesInstalledResult(output);
}

bool PkgSrc::packageVersionRequest(QString pkg, QString cat)
{
    bool status = false;
    if (!pkg.isEmpty()&&!cat.isEmpty()) {
        QStringList args;
        args << "show-var";
        int uid = 0;
        uid = getuid();
        QString bmake_exec;
        QString workdir;

        if (uid>0) {
            workdir = QDir::homePath()+"/userpkg/pkgsrc/"+cat+"/"+pkg;
            bmake_exec = QDir::homePath()+"/userpkg/pkg/bin/bmake";
        }
        else {
            workdir = "/usr/pkgsrc/"+cat+"/"+pkg;
            QFile native_bmake("/usr/bin/bmake");
            if (native_bmake.exists()) {
                bmake_exec = "/usr/bin/bmake";
            }
            else {
                bmake_exec = "/usr/pkg/bin/bmake";
            }
        }

        if (!bmake_exec.isEmpty()&&!workdir.isEmpty()) {
            QProcessEnvironment buildEnv = QProcessEnvironment::systemEnvironment();
            buildEnv.insert("VARNAME","PKGVERSION");
            pkgVersion->setProcessEnvironment(buildEnv);
            pkgVersion->setWorkingDirectory(workdir);
            pkgVersion->start(bmake_exec,args);
            status = true;
        }
    }

    return status;
}

void PkgSrc::pkgVersionDone(int status)
{
    QString value = pkgVersion->readAll();
    pkgVersion->close();
    QString output;
    if (status==0&&!value.isEmpty()) {
        output = value.simplified();
    }

    emit packageVersionResult(output);
}

bool PkgSrc::packageNameRequest(QString pkg, QString cat)
{
    bool status = false;
    if (!pkg.isEmpty()&&!cat.isEmpty()) {
        QStringList args;
        args << "show-var";
        int uid = 0;
        uid = getuid();
        QString bmake_exec;
        QString workdir;

        if (uid>0) {
            workdir = QDir::homePath()+"/userpkg/pkgsrc/"+cat+"/"+pkg;
            bmake_exec = QDir::homePath()+"/userpkg/pkg/bin/bmake";
        }
        else {
            workdir = "/usr/pkgsrc/"+cat+"/"+pkg;
            QFile native_bmake("/usr/bin/bmake");
            if (native_bmake.exists()) {
                bmake_exec = "/usr/bin/bmake";
            }
            else {
                bmake_exec = "/usr/pkg/bin/bmake";
            }
        }

        if (!bmake_exec.isEmpty()&&!workdir.isEmpty()) {
            QProcessEnvironment buildEnv = QProcessEnvironment::systemEnvironment();
            buildEnv.insert("VARNAME","PKGBASE");
            pkgName->setProcessEnvironment(buildEnv);
            pkgName->setWorkingDirectory(workdir);
            pkgName->start(bmake_exec,args);
            status = true;
        }
    }

    return status;
}

void PkgSrc::pkgNameDone(int status)
{
    QString value = pkgName->readAll();
    pkgName->close();
    QString output;
    if (status==0&&!value.isEmpty()) {
        output = value.simplified();
    }

    emit packageNameResult(output);
}

void PkgSrc::packagesVulnsRequest()
{
    int uid = 0;
    uid = getuid();
    QString workdir;

    if (uid>0) {
        workdir = QDir::homePath()+"/userpkg/pkg/sbin/";
    }
    else {
        workdir = "/usr/pkg/sbin/";
    }

    QStringList args;
    args<<"fetch-pkg-vulnerabilities";
    pkgVulnDownload->start(workdir+"pkg_admin",args);
}

void PkgSrc::pkgVulnDownloadDone(int status)
{
    if (status==0) {
        int uid = 0;
        uid = getuid();
        QString workdir;

        if (uid>0) {
            workdir = QDir::homePath()+"/userpkg/pkg/sbin/";
        }
        else {
            workdir = "/usr/pkg/sbin/";
        }

        QStringList args;
        args<<"audit";
        pkgVulnCheck->start(workdir+"pkg_admin",args);
    }
}

void PkgSrc::pkgVulnCheckDone(int status)
{
    QStringList pkgs;
    QString vulns = pkgVulnCheck->readAll();
    if (status==1) {
        QStringList vulnList = vulns.split("\n",QString::SkipEmptyParts);
        if (!vulnList.isEmpty()) {
            for (int i = 0; i < vulnList.size(); ++i) {
                QString vulnPkg = vulnList.at(i).left(vulnList.at(i).indexOf(",")).replace("Package ","");
                QString vulnUrl = vulnList.at(i).mid(vulnList.at(i).indexOf(",")).replace(", see ","");
                pkgs << vulnPkg+"|"+vulnUrl;
            }
        }
    }

    if (!pkgs.isEmpty()) {
        emit packagesVulnsResult(pkgs);
    }
}
