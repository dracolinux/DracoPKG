#ifndef DRACOPKG_H
#define DRACOPKG_H

#include <QMainWindow>
#include <QtDBus>
#include <QTimer>

namespace Ui {
class DracoPKG;
}

class DracoPKG : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit DracoPKG(QWidget *parent = 0);
    ~DracoPKG();
    
private:
    Ui::DracoPKG *ui;
private slots:
    bool pkgsrcConn();
    void pkgsrcDownloadStatus(qint64 start, qint64 end);
    void pkgsrcDownloadFinished(int status);
    void pkgsrcDownload();

    void pkgsrcExtractStatus(QString data);
    void pkgsrcExtractFinished(int status);
    void pkgsrcExtract();

    void pkgsrcBootstrapStatus(QString data);
    void pkgsrcBootstrapFinished(int status);
    void pkgsrcBootstrap();
    void pkgsrcBootstrapClean();
    bool pkgsrcBootstrapActive();
    void pkgsrcBootstrapReconnect();

    void pkgsrcBmakeStatus(QString data);
    void pkgsrcBmakeFinished(int status);
    void pkgsrcBmake();
    void pkgsrcBmakeStop();
    bool pkgsrcBmakeActive();
    void pkgsrcBmakeReconnect();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
};

#endif // DRACOPKG_H
