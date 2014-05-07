#include <QtCore>
#include <dracopkg.h>
#include <iostream>

class Bootstrap : public QObject
{
    Q_OBJECT
public:
    Bootstrap(QObject *parent = 0) : QObject(parent) {}
public slots:
    void run()
    {
        connect(&pkg,SIGNAL(pkgsrcReady()),this,SLOT(done()));
        connect(&pkg,SIGNAL(downloadStarted()),this,SLOT(downloadStarted()));
        connect(&pkg,SIGNAL(downloadFinished(int)),this,SLOT(downloadFinished(int)));
        connect(&pkg,SIGNAL(extractStarted()),this,SLOT(extractStarted()));
        connect(&pkg,SIGNAL(extractFinished(int)),this,SLOT(extractFinished(int)));
        connect(&pkg,SIGNAL(bootstrapStarted()),this,SLOT(bootstrapStarted()));
        connect(&pkg,SIGNAL(bootstrapFinished(int)),this,SLOT(bootstrapFinished(int)));
        pkg.initPkgsrc();
    }
    void downloadStarted()
    {
        std::cout<<"===> downloading pkgsrc"<<std::endl;
    }
    void downloadFinished(int status)
    {
        if(status==1)
            std::cout<<"===> pkgsrc downloaded"<<std::endl;
        else {
            std::cout<<"===> pkgsrc download failed!"<<std::endl;
            done();
        }
    }
    void extractStarted()
    {
        std::cout<<"===> extracting pkgsrc"<<std::endl;
    }
    void extractFinished(int status)
    {
        if (status==0)
            std::cout<<"===> pkgsrc extracted"<<std::endl;
        else {
            std::cout<<"===> pkgsrc extract failed"<<std::endl;
            done();
        }
    }
    void bootstrapStarted()
    {
        std::cout<<"===> bootstrap started"<<std::endl;
    }
    void bootstrapFinished(int status)
    {
        if (status==0)
            std::cout<<"===> bootstrap finished"<<std::endl;
        else {
            std::cout<<"===> bootstrap failed"<<std::endl;
            done();
        }
    }
    void done()
    {
        this->deleteLater();
        emit quit();
    }
signals:
    void quit();
private:
    PkgSrc pkg;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("DracoPKG");
    QCoreApplication::setApplicationVersion("test");
    QCoreApplication::setOrganizationDomain("dracolinux.org");
    QCoreApplication::setOrganizationName("DracoLinux");
    Bootstrap *bootstrap = new Bootstrap(&a);
    QObject::connect(bootstrap,SIGNAL(quit()),&a,SLOT(quit()));
    QTimer::singleShot(0,bootstrap,SLOT(run()));
    return a.exec();
}
#include "main.moc"
