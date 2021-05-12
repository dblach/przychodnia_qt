#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Ustawienia_clicked();
    void on_btnLekarze_clicked();
    void on_btnPacjenci_clicked();
    void on_btnPoradnie_clicked();
    void on_btnAplikacja_clicked();
    void on_btnAdministratorzy_clicked();
    void on_btnMoiPacjenci_clicked();

    void on_btnMojeWizyty_clicked();

private:
    QSettings st;
    Ui::MainWindow *ui;
    int uid;
    void setup();
    bool checkCredentials();
    bool setupSql();
    QString getRole();
    void uisetup(QString r);
    int get_uid();
};
#endif // MAINWINDOW_H


// icons made by flaticon @ freepik
