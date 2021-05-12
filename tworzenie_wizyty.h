#ifndef TWORZENIE_WIZYTY_H
#define TWORZENIE_WIZYTY_H

#include <QDialog>
#include <QSettings>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class tworzenie_wizyty;
}

class tworzenie_wizyty : public QDialog
{
    Q_OBJECT

public:
    explicit tworzenie_wizyty(QWidget *parent = nullptr,int idl=0,int idp=0,QDate dt=QDate::currentDate());
    ~tworzenie_wizyty();

private slots:
    void on_poradnia_currentTextChanged(const QString &t);
    void on_lekarz_currentTextChanged(const QString &t);
    void on_rbDoLekarza_clicked();
    void on_rbDoPoradni_clicked();
    void on_data_dateChanged();
    void on_lekarz_currentIndexChanged(const QString &t);
    void on_poradnia_currentIndexChanged(const QString &arg1);
    void on_tGrafik_cellDoubleClicked(int r,int c);
    void on_btnAnuluj_clicked();
    void on_btnZapisz_clicked();
    bool czyLekarzPrzyjmuje();
    bool czyTerminWolny();

private:
    Ui::tworzenie_wizyty *ui;
    void setup(int idl,int idp);
    QSettings st;
    void wczytaj_lekarzy();
    void wczytaj_poradnie();
    void wczytaj_grafik(int r);
    void lekarzChanged(QString t);
    void poradniaChanged(QString t);
    int lekarz_id=0;
    QString df="yyyy-MM-dd";
    QString tf="HH:mm";
};

#endif // TWORZENIE_WIZYTY_H
