#ifndef MOI_PACJENCI_H
#define MOI_PACJENCI_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class moi_pacjenci;
}

class moi_pacjenci : public QWidget
{
    Q_OBJECT

public:
    explicit moi_pacjenci(QWidget *parent = nullptr,int idl=0);
    ~moi_pacjenci();

private slots:
    void on_btnOdswiez_clicked();
    void on_tPacjenci_cellDoubleClicked(int r);

    void on_filtr_textChanged(const QString &arg1);

    void on_btnCzysc_clicked();

private:
    Ui::moi_pacjenci *ui;
    int idl;
    void setup();
};

#endif // MOI_PACJENCI_H
