#ifndef USTAWIENIA_APLIKACJI_H
#define USTAWIENIA_APLIKACJI_H

#include <QWidget>
#include <QtSql>
#include <QtNetwork>
#include <QMessageBox>
#include <QFileDialog>

namespace Ui {
class ustawienia_aplikacji;
}

class ustawienia_aplikacji : public QWidget
{
    Q_OBJECT

public:
    explicit ustawienia_aplikacji(QWidget *parent = nullptr);
    ~ustawienia_aplikacji();

private slots:
    void on_btnAnuluj_clicked();
    void on_btnZapisz_clicked();
    void on_btnZmienLogo_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_tAktualnosci_cellDoubleClicked(int r);

    void on_btnDodaj_clicked();

private:
    Ui::ustawienia_aplikacji *ui;
    QSettings st;
    QString logo="";
    bool logo_zmienione=false;
    QString logo_n="";
    QString get(QString n);
    bool set(QString k,QString v);
    void pobierz_ustawienia();
    void setImage(QNetworkReply *r);
    void wczytaj_aktualnosci();
    void otworz_edytor(int i);
};

#endif // USTAWIENIA_APLIKACJI_H
