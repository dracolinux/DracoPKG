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

    if (getuid()!=0)
    {
        qWarning("Cannot run as user");
        exit(1);
    }

    if (!QDBusConnection::systemBus().isConnected())
    {
        qWarning("Cannot connect to the D-Bus system bus.");
        return 1;
    }

    if (!QDBusConnection::systemBus().registerService("org.dracolinux.DracoPKG"))
    {
        qWarning() << QDBusConnection::systemBus().lastError().message();
        exit(1);
    }

    QDBusConnection::systemBus().registerObject("/pkgsrc", &dracopkg.pkgsrc, QDBusConnection::ExportAllContents);
    QDBusConnection::systemBus().registerObject("/pkgyum", &dracopkg.pkgyum, QDBusConnection::ExportAllContents);

    return a.exec();
}
