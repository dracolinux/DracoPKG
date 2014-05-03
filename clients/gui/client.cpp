#include "client.h"
#include "ui_client.h"

DracoPKG::DracoPKG(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DracoPKG)
{
    ui->setupUi(this);

    if (!pkgsrcConn())
    { // Connect one more time in case DracoPKG is not running
        QTimer::singleShot(1000,this,SLOT(pkgsrcConn()));
    }
}

DracoPKG::~DracoPKG()
{
    delete ui;
}

bool DracoPKG::pkgsrcConn()
{
    bool status = false;
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            status = true;
            if (pkgsrcBmakeActive())
            { // Reconnect to open bmake session
                pkgsrcBmakeReconnect();
            }
            if (pkgsrcBootstrapActive())
            { // Reconnect to open bootstrap session
                pkgsrcBootstrapReconnect();
            }
        }
    }
    return status;
}

void DracoPKG::pkgsrcDownload()
{ // Download pkgsrc tarball
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","downloadFinished",this,SLOT(pkgsrcDownloadFinished(int)));
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","downloadStatus",this,SLOT(pkgsrcDownloadStatus(qint64,qint64)));
            QDBusReply<bool> reply = iface.call("downloadStart");
            if (reply)
            {
                ui->statusBar->showMessage("Starting download of pkgsrc ...");
            }
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
}

void DracoPKG::pkgsrcDownloadFinished(int status)
{ // pkgsrc tarball download finished
    if (status ==1)
    {
        ui->statusBar->showMessage("pkgsrc download done");
    }
    else
    {
        ui->statusBar->showMessage("pkgsrc download failed");
    }
}

void DracoPKG::pkgsrcDownloadStatus(qint64 start, qint64 end)
{ // Get pkgsrc tarball download status
    ui->statusBar->showMessage("Downloading pkgsrc: "+QString::number(start)+" of "+QString::number(end));
}

void DracoPKG::pkgsrcExtract()
{ // Extract pkgsrc tarball
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","extractFinished",this,SLOT(pkgsrcExtractFinished(int)));
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","extractStatus",this,SLOT(pkgsrcExtractStatus(qint64,qint64)));
            QDBusReply<bool> reply = iface.call("extractStart");
            if (reply)
            {
                ui->statusBar->showMessage("Extracting pkgsrc ...");
            }
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
}

void DracoPKG::pkgsrcExtractFinished(int status)
{ // Extract pkgsrc tarball finished
    if (status ==0)
    {
        ui->statusBar->showMessage("pkgsrc extracted");
    }
    else
    {
        ui->statusBar->showMessage("failed to extract pkgsrc");
    }
}

void DracoPKG::pkgsrcExtractStatus(QString data)
{ // Extract pkgsrc tarball status
    ui->statusBar->showMessage(data);
}

void DracoPKG::pkgsrcBootstrap()
{ // Bootstrap pkgsrc
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","bootstrapFinished",this,SLOT(pkgsrcBootstrapFinished(int)));
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","bootstrapStatus",this,SLOT(pkgsrcBootstrapStatus(QString)));
            QDBusReply<bool> reply = iface.call("bootstrapStart");
            if (reply)
            {
                ui->statusBar->showMessage("Bootstrapping pkgsrc ...");
            }
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
}

void DracoPKG::pkgsrcBootstrapFinished(int status)
{ // Bootstrap pkgsrc finished
    if (status ==0)
    {
        ui->statusBar->showMessage("Bootstrap finished");
    }
    else
    {
        ui->statusBar->showMessage("Bootstrap failed");
    }
}

void DracoPKG::pkgsrcBootstrapStatus(QString data)
{ // Bootstrap pkgsrc status
    if (data.contains("===>")) {
        QString status = data;
        ui->statusBar->showMessage(status.replace("=","").replace(">",""));
    }
    ui->plainTextEdit->appendPlainText(data);
}

void DracoPKG::pkgsrcBootstrapClean()
{ // Clean pkgsrc bootstrap work dir
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            QDBusReply<bool> reply = iface.call("bootstrapClean");
            if (reply)
            {
                ui->statusBar->showMessage("Boostrap cleaned");
            }
            else
            {
                ui->statusBar->showMessage("Failed to clean bootstrap");
            }
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
}

bool DracoPKG::pkgsrcBootstrapActive()
{
    bool status = false;
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            QDBusReply<bool> reply = iface.call("bootstrapActive");
            if (reply)
            {
                status = true;
            }
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
    return status;
}

void DracoPKG::pkgsrcBootstrapReconnect()
{
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","bootstrapFinished",this,SLOT(pkgsrcBootstrapFinished(int)));
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","bootstrapStatus",this,SLOT(pkgsrcBootstrapStatus(QString)));
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
}

void DracoPKG::pkgsrcBmake()
{ // build pkgsrc package
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","bmakeFinished",this,SLOT(pkgsrcBmakeFinished(int)));
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","bmakeStatus",this,SLOT(pkgsrcBmakeStatus(QString)));
            QDBusReply<bool> reply = iface.call("bmakeStart",ui->lineEdit->text(),ui->lineEdit_2->text(),"","install");
            if (reply)
            {
                ui->statusBar->showMessage("Starting bmake ...");
            }
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
}

void DracoPKG::pkgsrcBmakeFinished(int status)
{ // Build pkgsrc package finished
    if (status ==0)
    {
        ui->statusBar->showMessage("Bmake finished");
    }
    else
    {
        ui->statusBar->showMessage("Bmake failed");
    }
}

void DracoPKG::pkgsrcBmakeStatus(QString data)
{ // Build pkgsrc package status
    if (data.contains("===>")) {
        QString status = data;
        ui->statusBar->showMessage(status.replace("=","").replace(">",""));
    }
    ui->plainTextEdit->appendPlainText(data);
}

void DracoPKG::pkgsrcBmakeStop()
{ // Build pkgsrc package stop
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            QDBusReply<bool> reply = iface.call("bmakeStop");
            if (reply)
            {
                ui->statusBar->showMessage("Bmake stopped");
            }
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
}

bool DracoPKG::pkgsrcBmakeActive()
{ // Is bmake running?
    bool status = false;
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            QDBusReply<bool> reply = iface.call("bmakeActive");
            if (reply)
            {
                status = true;
            }
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
    return status;
}

void DracoPKG::pkgsrcBmakeReconnect()
{ // Reconnect bmake session
    QDBusConnection bus = QDBusConnection::systemBus();
    if (bus.isConnected())
    {
        QDBusInterface iface("org.dracolinux.DracoPKG", "/pkgsrc", "", bus);
        if (iface.isValid())
        {
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","bmakeFinished",this,SLOT(pkgsrcBmakeFinished(int)));
            bus.connect("org.dracolinux.DracoPKG","/pkgsrc","org.dracolinux.DracoPKG.PkgSrc","bmakeStatus",this,SLOT(pkgsrcBmakeStatus(QString)));
        }
    }
    else
    {
        qFatal("Cannot connect to the D-Bus session bus.");
    }
}


















// TMP


void DracoPKG::on_pushButton_clicked()
{
    pkgsrcDownload();
}

void DracoPKG::on_pushButton_2_clicked()
{
    pkgsrcExtract();
}

void DracoPKG::on_pushButton_3_clicked()
{
    pkgsrcBootstrap();
}

void DracoPKG::on_pushButton_4_clicked()
{
    if (!pkgsrcBmakeActive())
    {
        pkgsrcBmake();
    }
}

void DracoPKG::on_pushButton_5_clicked()
{
    if (pkgsrcBmakeActive())
    {
        pkgsrcBmakeStop();
    }
}

void DracoPKG::on_pushButton_6_clicked()
{
    pkgsrcBootstrapClean();
}
