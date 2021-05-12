#ifndef EDYCJA_WIZYTY_H
#define EDYCJA_WIZYTY_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class edycja_wizyty;
}

class edycja_wizyty : public QDialog
{
    Q_OBJECT

public:
    explicit edycja_wizyty(QWidget *parent = nullptr,int id=0);
    ~edycja_wizyty();

private slots:
    void on_btnAnuluj_clicked();
    void on_btnOk_clicked();
    void on_btnOdwolaj_clicked();

private:
    Ui::edycja_wizyty *ui;
    int id;
    QString df="yyyy-MM-dd";
    QString tf="HH:mm:ss";
    QVector<QMap<QString,QString>> d;
};

#endif // EDYCJA_WIZYTY_H
