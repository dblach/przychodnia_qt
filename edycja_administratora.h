#ifndef EDYCJA_ADMINISTRATORA_H
#define EDYCJA_ADMINISTRATORA_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class edycja_administratora;
}

class edycja_administratora : public QDialog
{
    Q_OBJECT

public:
    explicit edycja_administratora(QWidget *parent = nullptr,int id=0);
    ~edycja_administratora();

private slots:
    void on_btnUsun_clicked();
    void on_btnZapisz_clicked();
    void on_btnAnuluj_clicked();

private:
    Ui::edycja_administratora *ui;
    int id;
};

#endif // EDYCJA_ADMINISTRATORA_H
