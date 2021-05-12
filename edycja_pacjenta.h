#ifndef EDYCJA_PACJENTA_H
#define EDYCJA_PACJENTA_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class edycja_pacjenta;
}

class edycja_pacjenta : public QDialog
{
    Q_OBJECT

public:
    explicit edycja_pacjenta(QWidget *parent = nullptr,int id=0,bool allow_editing=true);
    ~edycja_pacjenta();

private slots:
    void on_btnZapisz_clicked();
    void on_btnUsun_clicked();
    void on_btnAnuluj_clicked();
    void on_tabWidget_currentChanged(int index);
    void wczytaj_tydzien();
    void on_btnPoprzedni_clicked();
    void on_btnNastepny_clicked();
    void zmien_tydzien(int t);
    void on_tWizyty_cellDoubleClicked(int r,int dt);
    void on_btnDodajWizyte_clicked();
    void on_btnData_clicked();

private:
    Ui::edycja_pacjenta *ui;
    void load_data(int id);
    QVector<QMap<QString,QString>> d;
    QStringList l;
    QSettings st;
    QString df="dd.MM.yyyy";
};

#endif // EDYCJA_PACJENTA_H
