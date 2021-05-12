#ifndef EDYCJA_TERMINU_H
#define EDYCJA_TERMINU_H

#include <QDialog>
#include <QTime>

namespace Ui {
class edycja_terminu;
}

class edycja_terminu : public QDialog
{
    Q_OBJECT

public:
    explicit edycja_terminu(QWidget *parent = nullptr,int idl=0,int idt=0,int dt=0,QTime gp=QTime(0,0,0));
    ~edycja_terminu();

private slots:
    void on_btnAnuluj_clicked();
    void on_btnOk_clicked();
    void on_btnUsun_clicked();

private:
    Ui::edycja_terminu *ui;
    void setup(int idl=0);
    void getLekarze();
    void getSpecjalizacje();
    int sprawdzTermin(QString idl,QString dzien,QString gp,QString gk);
};

#endif // EDYCJA_TERMINU_H
