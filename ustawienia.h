#ifndef USTAWIENIA_H
#define USTAWIENIA_H

#include <QDialog>
#include <QSettings>
#include <QColorDialog>
#include <QApplication>
#include <QProcess>

namespace Ui {
class ustawienia;
}

class ustawienia : public QDialog
{
    Q_OBJECT

public:
    explicit ustawienia(QWidget *parent = nullptr);
    ~ustawienia();

private slots:
    void on_btnOk_clicked();
    void on_btnZwykly_tekst_clicked();
    void on_btnZwykly_tlo_clicked();
    void on_btnWolny_tekst_clicked();
    void on_btnWolny_tlo_clicked();
    void on_btnZajety_tekst_clicked();
    void on_btnZajety_tlo_clicked();
    void on_btnCancel_clicked();

private:
    QSettings s;
    Ui::ustawienia *ui;
    void getSettings();
    void saveSettings();
    QString ustaw_kolor();

};

#endif // USTAWIENIA_H
