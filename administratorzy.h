#ifndef ADMINISTRATORZY_H
#define ADMINISTRATORZY_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class administratorzy;
}

class administratorzy : public QWidget
{
    Q_OBJECT

public:
    explicit administratorzy(QWidget *parent = nullptr);
    ~administratorzy();

private slots:
    void on_tAdministratorzy_cellDoubleClicked(int r);

    void on_btnDodaj_clicked();

private:
    Ui::administratorzy *ui;
    void get();
    void otworz_edytor(int id);
};

#endif // ADMINISTRATORZY_H
