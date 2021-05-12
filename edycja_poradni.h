#ifndef EDYCJA_PORADNI_H
#define EDYCJA_PORADNI_H

#include <QDialog>
#include <QtSql>
#include <QtNetwork>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class edycja_poradni;
}

class edycja_poradni : public QDialog
{
    Q_OBJECT

public:
    explicit edycja_poradni(QWidget *parent = nullptr,int idp=0);
    ~edycja_poradni();

private slots:
    void on_btnZmienZdjecie_clicked();
    void on_btnZapisz_clicked();
    void on_btnAnuluj_clicked();

private:
    Ui::edycja_poradni *ui;
    int idp;
    QString zdj="";
    bool zdj_zmienione=false;
    QString zdj_n="";
    QSettings st;
    void setup();
    void setImage(QNetworkReply *r);
};

#endif // EDYCJA_PORADNI_H
