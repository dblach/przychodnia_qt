#ifndef PACJENCI_H
#define PACJENCI_H

#include <QWidget>

namespace Ui {
class pacjenci;
}

class pacjenci : public QWidget
{
    Q_OBJECT

public:
    explicit pacjenci(QWidget *parent = nullptr);
    ~pacjenci();

private slots:
    void on_filtr_textChanged(const QString &arg1);
    void on_btnCzysc_clicked();
    void on_btnNowy_clicked();
    void on_tPacjenci_cellDoubleClicked(int r);

private:
    Ui::pacjenci *ui;
    void setup();
};

#endif // PACJENCI_H
