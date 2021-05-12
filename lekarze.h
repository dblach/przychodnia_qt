#ifndef LEKARZE_H
#define LEKARZE_H

#include <QWidget>
#include <QSettings>
namespace Ui {
class lekarze;
}

class lekarze : public QWidget
{
    Q_OBJECT

public:
    explicit lekarze(QWidget *parent = nullptr);
    ~lekarze();

private slots:
    void on_tabWidget_currentChanged(int index);
    void on_tWgNazwisk_filtr_textChanged();
    void on_tWgNazwisk_filtr_czysc_clicked();
    void on_tWgGodzinPrzyjec_dzien_currentIndexChanged();
    void on_tWgNazwisk_btnNowy_clicked();
    void on_tWgNazwisk_table_cellDoubleClicked(int r);

private:
    Ui::lekarze *ui;
    QSettings st;
    void setupWgNazwisk();
    void setupWgGodzinPrzyjec();
    void tWgNazwisk_filtruj();
};

#endif // LEKARZE_H
