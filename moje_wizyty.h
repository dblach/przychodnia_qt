#ifndef MOJE_WIZYTY_H
#define MOJE_WIZYTY_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class moje_wizyty;
}

class moje_wizyty : public QWidget
{
    Q_OBJECT

public:
    explicit moje_wizyty(QWidget *parent=nullptr,int uid=0);
    ~moje_wizyty();

private slots:
    void on_btnPoprzedni_clicked();
    void on_btnNastepny_clicked();
    void on_btnData_clicked();
    void on_btnOdswiez_clicked();
    void on_tWizyty_cellDoubleClicked(int r,int dt);

private:
    Ui::moje_wizyty *ui;
    int id;
    QSettings st;
    void zmien_date(int n);
    void wczytaj_wizyty();
    QVector<QMap<QString,QString>> d;
};

#endif // MOJE_WIZYTY_H
