#ifndef EDYCJA_AKTUALNOSCI_H
#define EDYCJA_AKTUALNOSCI_H

#include <QDialog>
#include <QDate>
#include <QtSql>
#include <QtNetwork>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class edycja_aktualnosci;
}

class edycja_aktualnosci : public QDialog
{
    Q_OBJECT

public:
    explicit edycja_aktualnosci(QWidget *parent = nullptr,int id=0);
    ~edycja_aktualnosci();

private slots:
    void on_btnZmienZdjecie_clicked();

    void on_btnAnuluj_clicked();

    void on_btnZapisz_clicked();

    void on_btnUsun_clicked();

private:
    Ui::edycja_aktualnosci *ui;
    int id;
    QSettings st;
    QString zdj="";
    QString zdj_n="";
    bool zdj_zmienione=false;
    void setImage(QNetworkReply *r);
};

#endif // EDYCJA_AKTUALNOSCI_H
