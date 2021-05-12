#ifndef KALENDARZ_H
#define KALENDARZ_H

#include <QDialog>

namespace Ui {
class kalendarz;
}

class kalendarz : public QDialog
{
    Q_OBJECT

public:
    explicit kalendarz(QWidget *parent = nullptr);
    ~kalendarz();
    QDate getDate();

private slots:
    void on_btnAnuluj_clicked();
    void on_btnOk_clicked();

private:
    Ui::kalendarz *ui;
};

#endif // KALENDARZ_H
