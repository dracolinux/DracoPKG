/*
# libDracoPKG - Package management library.
#
# Copyright (c) 2014 Ole Andre Rodlie <olear@dracolinux.org>. All rights reserved.
#
# libDracoPKG is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License version 2.1.
*/

#include "pkgsrc.h"

PkgSrc::PkgSrc(QObject *parent) :
    QObject(parent)
{
    initPkgsrcThreads = 0;
    initPkgsrcCC = "";
    initPkgsrcBranch = 0;
    initPkgsrcOptions = "";
    pkgsrcDownload = new QFtp(this);
    connect(pkgsrcDownload,SIGNAL(done(bool)),SLOT(downloadDone()));
    connect(pkgsrcDownload,SIGNAL(dataTransferProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));

    pkgsrcExtract = new QProcess(this);
    pkgsrcExtract->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgsrcExtract,SIGNAL(started()),this,SLOT(extractStartEmit()));
    connect(pkgsrcExtract,SIGNAL(finished(int)),this,SLOT(extractDone(int)));
    connect(pkgsrcExtract,SIGNAL(readyRead()),this,SLOT(extractProgress()));

    pkgsrcBootstrap = new QProcess(this);
    pkgsrcBootstrap->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgsrcBootstrap,SIGNAL(started()),this,SLOT(bootstrapStartEmit()));
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

    pkgRemove = new QProcess(this);
    pkgRemove->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgRemove,SIGNAL(finished(int)),this,SLOT(pkgRemoveDone(int)));
    connect(pkgRemove,SIGNAL(readyRead()),this,SLOT(pkgRemoveProgress()));

    pkgsrcSync = new QProcess(this);
    pkgsrcSync->setProcessChannelMode(QProcess::MergedChannels);
    connect(pkgsrcSync,SIGNAL(finished(int)),this,SLOT(pkgsrcSyncDone(int)));
    connect(pkgsrcSync,SIGNAL(readyRead()),this,SLOT(pkgsrcSyncProgress()));
}

bool PkgSrc::downloadStart()
{
    QString tmp_dir = pkgHome()+"/tmp";
    QDir tmp;
    if (!tmp.exists(tmp_dir)) {
        tmp.mkdir(tmp_dir);
    }

    pkgsrcDownloadFile = new QFile(tmp_dir+"/pkgsrc.tar.xz");
    pkgsrcDownloadMd5 = new QFile(tmp_dir+"/pkgsrc.tar.xz.MD5");

    if (pkgsrcDownloadMd5->open(QIODevice::WriteOnly) && pkgsrcDownloadFile->open(QIODevice::WriteOnly)) {
        pkgsrcDownload->connectToHost("ftp.netbsd.org");
        pkgsrcDownload->login("ftp","ftp");
        pkgsrcDownload->cd("pub");
        pkgsrcDownload->cd("pkgsrc");
        if (initPkgsrcBranch==1)
            pkgsrcDownload->cd("current");
        else
            pkgsrcDownload->cd("stable");
        pkgsrcDownload->get("pkgsrc.tar.xz", pkgsrcDownloadFile);
        pkgsrcDownload->get("pkgsrc.tar.xz.MD5", pkgsrcDownloadMd5);
        pkgsrcDownload->close();
        emit downloadStarted();
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
    QStringList pkgsrc = categoryList();
    QString pkgsrc_tarball = pkgHome()+"/tmp/pkgsrc.tar.xz";
    QFile pkgsrcTar(pkgsrc_tarball);

    if (!pkgsrc.isEmpty()) {
        dirClean(pkgHome()+"/pkgsrc");
    }

    if (pkgsrcTar.exists(pkgsrc_tarball) && !pkgsrcBootstrap->isOpen() && !pkgsrcBmake->isOpen()) {
        QStringList args;
        args << "xvf" << pkgsrc_tarball << "-C" << pkgHome();
        QString tar = "tar";
        //if (Q_OS_NetBSD)
          //  tar = "gtar";
        pkgsrcExtract->start(tar,args);
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

void PkgSrc::extractStartEmit()
{
    emit extractStarted();
}

bool PkgSrc::bootstrapStart()
{
    int jobs = 2;
    if (initPkgsrcThreads>0 && initPkgsrcThreads!=jobs)
        jobs=initPkgsrcThreads;

    bool status = false;
    QString bmake_exec = bmakeExec();
    QString workdir = pkgHome()+"/pkgsrc/bootstrap";

    QFile bmake(bmake_exec);
    QDir bootstrap_dir(workdir);
    if (!bmake.exists()&&bootstrap_dir.exists(workdir)&&!pkgsrcBootstrap->isOpen()&&!pkgsrcBmake->isOpen()&&!pkgsrcSync->isOpen()) {
        QStringList args;
        QString mkIncFileName = "mk-fragment.conf";
        QFile mkIncFile(workdir+"/"+mkIncFileName);
        if (mkIncFile.exists(workdir+"/"+mkIncFileName)) {
            mkIncFile.remove();
        }
        if (mkIncFile.open(QIODevice::WriteOnly)) {
            args << "--mk-fragment="+mkIncFileName;
            QTextStream textStream(&mkIncFile);
            textStream << "MAKE_JOBS = "+QString::number(jobs)+"\n";
            textStream << "SKIP_LICENSE_CHECK = yes\n";
            textStream << "ALLOW_VULNERABLE_PACKAGES = yes\n";
            textStream << "FAILOVER_FETCH = yes\n";
            textStream << "FAM_DEFAULT = gamin\n";
            textStream << "X11_TYPE = modular\n";
            textStream << "PREFER.ncurses=native\n"; // Maybe use pkgsrc ncurses?
            if (!initPkgsrcCC.isEmpty()) {
                textStream << "GCC_REQD+="+initPkgsrcCC+"\n";
                textStream << ".for GCCPKG in lang/gcc"+initPkgsrcCC+" pkgtools/digest archivers/pax converters/p5-Unicode-EastAsianWidth textproc/p5-Text-Unidecode converters/help2man misc/p5-Locale-libintl devel/p5-gettext devel/nbpatch devel/gettext-lib devel/libtool-base archivers/bzip2 archivers/zip archivers/unzip converters/libiconv devel/bison devel/flex devel/gmp devel/autoconf devel/gettext-tools devel/gmake devel/gtexinfo devel/m4 devel/zlib lang/perl5 math/mpfr net/tnftp pkgtools/pkg_install-info sysutils/checkperms\n";
                textStream << ".       if ${PKGPATH} == ${GCCPKG}\n";
                textStream << "GCC_REQD=\n";
                textStream << ".       endif\n";
                textStream << ".endfor\n";
            }
            if (!initPkgsrcOptions.isEmpty()) {
                textStream << "PKG_DEFAULT_OPTIONS+="+initPkgsrcOptions+"\n";
            }
            mkIncFile.close();
        }

        QProcessEnvironment buildEnv = QProcessEnvironment::systemEnvironment();
        QFile bash("/bin/bash");
        if (bash.exists()) {
            buildEnv.insert("SH","/bin/bash");
        }

        int uid = 0;
        uid = getuid();
        if (uid>0) {
            args << "--pkgdbdir="+QDir::homePath()+"/.pkgdb" << "--prefix="+QDir::homePath()+"/pkg" << "--unprivileged";
        }
        else {
            args << "--prefix=/usr/pkg";
        }
        args  <<  "--prefer-pkgsrc=yes";

        bootstrapClean();
        pkgsrcBootstrap->setWorkingDirectory(workdir);
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
    //bmakeStart("pkgclean","pkgtools","","install");

    emit bootstrapFinished(status);
    if (status==0) {
      emit pkgsrcReady();
    }
}

void PkgSrc::bootstrapProgress()
{
    emit bootstrapStatus(pkgsrcBootstrap->readAll());
}

bool PkgSrc::bootstrapClean()
{
    QString dir = pkgHome()+"/pkgsrc/bootstrap/work";
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

void PkgSrc::bootstrapStartEmit()
{
    emit bootstrapStarted();
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
    if (!pkg.isEmpty()&&!cat.isEmpty()&&!action.isEmpty()&&!pkgsrcBmake->isOpen()&&!pkgsrcBootstrap->isOpen()&&!pkgsrcSync->isOpen()) {
        QStringList args;
        args << action;
        if (action == "install") {
            args << "clean" << "clean-depends";
        }
        int uid = 0;
        uid = getuid();
        QString bmake_exec = bmakeExec();
        QString workdir = pkgHome()+"/pkgsrc/"+cat+"/"+pkg;

        QProcessEnvironment buildEnv = QProcessEnvironment::systemEnvironment();
        if (!options.isEmpty()) {
            buildEnv.insert("PKG_OPTIONS."+pkg, options);
        }

        QFile bmake(bmake_exec);
        QDir pkgsrc(workdir);
        if (bmake.exists(bmake_exec)&&pkgsrc.exists(workdir)) {
            pkgsrcBmake->setProcessEnvironment(buildEnv);
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
        if (!pkgsrcBmake->isOpen()) {
            status = true;
        }
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
    QString pkgsrcDir = pkgHome()+"/pkgsrc";
    QString tagFile = pkgsrcDir+"/CVS/Tag";

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
        QFile pkgsrcMakefile;
        if (pkgsrCurrent.exists(pkgsrcDir)&&pkgsrcMakefile.exists(pkgsrcDir+"/Makefile")) {
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
        QString bmake_exec = bmakeExec();
        QString workdir = pkgHome()+"/pkgsrc/"+cat+"/"+pkg;

        QFile bmake(bmake_exec);
        QDir pkgsrc(workdir);
        if (bmake.exists(bmake_exec)&&pkgsrc.exists(workdir)) {
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
        QString bmake_exec = bmakeExec();
        QString workdir = pkgHome()+"/pkgsrc/"+cat+"/"+pkg;;

        QFile bmake(bmake_exec);
        QDir pkgsrc(workdir);
        if (bmake.exists(bmake_exec)&&pkgsrc.exists(workdir)) {
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
    QString workdir = pkgHome()+"/pkgsrc";

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
    QString workdir = pkgHome()+"/pkgsrc";
    QString searchdir = workdir;

    if (!cat.isEmpty()) {
        workdir = workdir+"/"+cat;
    }

    if (!search.isEmpty()) {
        QStringList categories = categoryList();
        for (int i = 0; i < categories.size(); ++i) {
            QDir pkgsrc(searchdir+"/"+categories.at(i));
            QFileInfoList pkgsrcList(pkgsrc.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs));
            foreach (QFileInfo folder, pkgsrcList) {
                if (folder.isDir()) {
                    QFile Makefile(folder.filePath()+"/Makefile");
                    if (Makefile.exists(folder.filePath()+"/Makefile") && folder.fileName().contains(search)) {
                        QString outputString;
                        outputString.append(categories.at(i));
                        outputString.append("|"+folder.fileName());
                        if(Makefile.open(QIODevice::ReadOnly)) {
                            QTextStream stream(&Makefile);
                            while(!stream.atEnd()) {
                                QString line = stream.readLine();
                                if (line.contains("COMMENT=")) {
                                    outputString.append( "|"+line.replace("COMMENT=","").trimmed());
                                }
                            }
                        }
                        output<<outputString;
                    }
                }
            }
        }
    }
    else {
        QDir pkgsrc(workdir);
        QFileInfoList pkgsrcList(pkgsrc.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs));
        foreach (QFileInfo folder, pkgsrcList) {
            if (folder.isDir()) {
                QFile Makefile(folder.filePath()+"/Makefile");
                if (Makefile.exists(folder.filePath()+"/Makefile")) {
                    QString outputString;
                    if (!cat.isEmpty()) {
                        outputString.append(cat+"|");
                    }
                    outputString.append(folder.fileName());
                    if(Makefile.open(QIODevice::ReadOnly)) {
                        QTextStream stream(&Makefile);
                        while(!stream.atEnd()) {
                            QString line = stream.readLine();
                            if (line.contains("COMMENT=")) {
                                outputString.append( "|"+line.replace("COMMENT=","").trimmed());
                            }
                        }
                    }
                    output<<outputString;
                }
            }
        }
    }

    return output;
}

void PkgSrc::packageCleanRequest()
{
    pkgClean->start(pkgHome()+"/pkg/bin/pkgclean");
}

void PkgSrc::pkgCleanDone(int status)
{
    pkgClean->close();
    emit packageCleanFinished(status);
}

void PkgSrc::packagesInstalledRequest()
{
    pkgInstalled->start(pkgHome()+"/pkg/sbin/pkg_info");
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
        QString bmake_exec = bmakeExec();
        QString workdir = pkgHome()+"/pkgsrc/"+cat+"/"+pkg;

        QFile bmake(bmake_exec);
        QDir pkgsrc(workdir);
        if (bmake.exists(bmake_exec)&&pkgsrc.exists(workdir)) {
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
        QString bmake_exec = bmakeExec();
        QString workdir = pkgHome()+"/pkgsrc/"+cat+"/"+pkg;

        QFile bmake(bmake_exec);
        QDir pkgsrc(workdir);
        if (bmake.exists(bmake_exec)&&pkgsrc.exists(workdir)) {
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
    QStringList args;
    args<<"fetch-pkg-vulnerabilities";
    pkgVulnDownload->start(pkgHome()+"/pkg/sbin/pkg_admin",args);
}

void PkgSrc::pkgVulnDownloadDone(int status)
{
    if (status==0) {
        QStringList args;
        args<<"audit";
        pkgVulnCheck->start(pkgHome()+"/pkg/sbin/pkg_admin",args);
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

QString PkgSrc::pkgHome()
{
    int uid = 0;
    uid = getuid();

    if (uid>0) {
        return QDir::homePath();
    }
    else {
        return "/usr";
    }
}

QString PkgSrc::bmakeExec()
{
    QString bmake_exec = pkgHome()+"/pkg/bin/bmake";
    int uid = -1;
    uid = getuid();

    if (uid==0) {
        QFile native_bmake("/usr/bin/bmake");
        if (native_bmake.exists()) {
            bmake_exec = "/usr/bin/bmake";
        }
    }

    return bmake_exec;
}

void PkgSrc::pkgRemoveDone(int status)
{
    emit packageRemoveResult(status);
}

void PkgSrc::pkgRemoveProgress()
{
    emit packageRemoveStatus(pkgRemove->readAll());
}

bool PkgSrc::packageRemove(QString pkg, int recursive)
{
    bool status = false;
    if (!pkg.isEmpty()) {
        status = true;
        QStringList args;
        args << "-v";
        if (recursive==1) {
            args << "-A";
        }
        args << pkg;
        pkgRemove->start(pkgHome()+"/pkg/sbin/pkg_delete",args);
    }
    return status;
}

void PkgSrc::initPkgsrc(int threads, QString gcc, int branch, QString options)
{
    qDebug() << threads << gcc << branch << options;
    if (threads>0)
        initPkgsrcThreads=threads;
    if (!gcc.isEmpty())
        initPkgsrcCC=gcc;
    if (!options.isEmpty())
        initPkgsrcOptions=options;

    if (branch == 1)
        initPkgsrcBranch=1;

    connect(this,SIGNAL(extractFinished(int)),this,SLOT(initPkgsrcBootstrap(int)));
    connect(this,SIGNAL(downloadFinished(int)),this,SLOT(extractStart()));

    QString pkgsrcBranch = branchVersion();
    if (pkgsrcBranch.isEmpty()) {
        QFile pkgsrcTar(pkgHome()+"/tmp/pkgsrc.tar.xz");
        if (pkgsrcTar.exists()) {
            extractStart();
        }
        else {
            downloadStart();
        }
    }
    else {
        initPkgsrcBootstrap(0);
    }
}

void PkgSrc::initPkgsrcBootstrap(int status)
{
    if (status == 0) {
        QFile bmake(bmakeExec());
        if (!bmake.exists(bmakeExec())) {
            bootstrapStart();
        }
        else {
            emit pkgsrcReady();
        }
    }
}

void PkgSrc::pkgsrcSyncDone(int status)
{
    pkgsrcSync->close();
    QFile askpass(pkgHome()+"/tmp/pkgsrc_accept_key.sh");
    if (askpass.exists()) {
        askpass.remove();
    }
    emit pkgsrcUpdateFinished(status);
}

void PkgSrc::pkgsrcSyncProgress()
{
    emit pkgsrcUpdateStatus(pkgsrcSync->readAll());
}

void PkgSrc::updatePkgsrc()
{ // TODO! check for cvs, build if req, ssh is also req, check for that as well
    if (!pkgsrcBmake->isOpen()&&!pkgsrcBootstrap->isOpen()) {
        QFile askpass(pkgHome()+"/tmp/pkgsrc_accept_key.sh");
        if (askpass.open(QIODevice::WriteOnly)) {
            QTextStream textStream(&askpass);
            textStream << "#!/bin/sh\n";
            textStream << "echo yes\n";
            askpass.close();
        }

        if (askpass.exists()) {
            QStringList args;
            args << "update" << "-dP";
            QProcessEnvironment cvsEnv = QProcessEnvironment::systemEnvironment();
            askpass.setPermissions(QFile::ExeOwner|QFile::ReadOwner|QFile::WriteOwner);
            cvsEnv.insert("CVS_RSH","ssh");
            cvsEnv.insert("SSH_ASKPASS",askpass.fileName());
            pkgsrcSync->setProcessEnvironment(cvsEnv);
            pkgsrcSync->setWorkingDirectory(pkgHome()+"/pkgsrc");
            pkgsrcSync->start("cvs", args);
        }
    }
}

QStringList PkgSrc::packageUpdates()
{
    QStringList output;
    QString workdir = pkgHome()+"/.pkgdb";
    QDir pkgsrc(workdir);
    QFileInfoList pkgsrcList(pkgsrc.entryInfoList(QDir::NoDotAndDotDot|QDir::AllDirs));
    foreach (QFileInfo folder, pkgsrcList) {
        QString pkgpath;
        QString pkgname;
        QString pkgversion;
        QString newpkgversion;
        if (folder.isDir()) {
            QFile Makefile(folder.filePath()+"/+BUILD_INFO");
            if (Makefile.exists()) {
                if(Makefile.open(QIODevice::ReadOnly)) {
                    QTextStream stream(&Makefile);
                    while(!stream.atEnd()) {
                        QString line = stream.readLine();
                        if (line.contains("PKGPATH=")) {
                            pkgpath = line.replace("PKGPATH=","").trimmed();
                            pkgversion = folder.fileName().mid(folder.fileName().lastIndexOf("-")).replace("-","");
                            pkgname = folder.fileName().left(folder.fileName().lastIndexOf("-"));
                            QDir pkgdir(pkgHome()+"/pkgsrc/"+pkgpath);
                            if (pkgdir.exists()&&!pkgversion.isEmpty()&&!pkgversion.contains("bmake")&&!pkgname.isEmpty()) {
                               QProcess pkg_info_proc;
                               QStringList pkg_info_args;
                               pkg_info_args << "show-var";
                               QProcessEnvironment buildEnv = QProcessEnvironment::systemEnvironment();
                               buildEnv.insert("VARNAME","PKGVERSION");
                               pkg_info_proc.setProcessEnvironment(buildEnv);
                               pkg_info_proc.setWorkingDirectory(pkgdir.absolutePath());
                               pkg_info_proc.setProcessChannelMode(QProcess::MergedChannels);
                               pkg_info_proc.start(pkgHome()+"/pkg/bin/bmake",pkg_info_args);
                               pkg_info_proc.waitForFinished(-1);
                               newpkgversion = pkg_info_proc.readAll().trimmed();
                               if (pkgversion != newpkgversion) {
                                   output << pkgpath+"|"+pkgname+"|"+pkgversion+"|"+newpkgversion;
                               }
                            }
                            qDebug() << pkgpath << pkgname << pkgversion << "vs." << newpkgversion;
                        }
                    }
                }
            }
        }
    }
    return output;
}
