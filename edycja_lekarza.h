#ifndef EDYCJA_LEKARZA_H
#define EDYCJA_LEKARZA_H

#include <QDialog>
#include <QSettings>
#include <QtNetwork>

namespace Ui {
class edycja_lekarza;
}

class edycja_lekarza : public QDialog
{
    Q_OBJECT

public:
    explicit edycja_lekarza(QWidget *parent = nullptr,int id=0);
    ~edycja_lekarza();

private slots:
    void on_btnAnuluj_clicked();
    void on_btnZapisz_clicked();
    void on_btnUsun_clicked();
    void on_btnZmienZdjecie_clicked();
    void on_tabWidget_tabBarClicked(int index);
    void on_tGodzinyPrzyjec_cellDoubleClicked(int r,int dt);
    void on_tabWidget_currentChanged(int index);
    void on_btnDodajTermin_clicked();

private:
    Ui::edycja_lekarza *ui;
    int idl=0;
    QSettings st;
    QString zdj="";
    bool zdj_zmienione=false;
    QString zdj_n="";
    QVector<QMap<QString,QString>> d;
    QStringList pobierz_specjalizacje();
    void setImage(QNetworkReply *r);
    void setupGodzinyPrzyjec();
    void setup(int id);
};

#endif // EDYCJA_LEKARZA_H
