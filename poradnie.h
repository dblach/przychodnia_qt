#ifndef PORADNIE_H
#define PORADNIE_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class poradnie;
}

class poradnie : public QWidget
{
    Q_OBJECT

public:
    explicit poradnie(QWidget *parent = nullptr);
    ~poradnie();

private slots:
    void on_tPoradnie_cellDoubleClicked(int r);

    void on_btnDodaj_clicked();

private:
    Ui::poradnie *ui;
    void get();
    void otworz_edytor(int i);
};

#endif // PORADNIE_H
