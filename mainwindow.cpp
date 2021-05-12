#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ustawienia.h"
#include "lekarze.h"
#include "pacjenci.h"
#include "poradnie.h"
#include "ustawienia_aplikacji.h"
#include "administratorzy.h"
#include "moje_wizyty.h"
#include "moi_pacjenci.h"
#include "strings.h"
#include "globals.h"
#include <QString>
#include <QMessageBox>
#include <QtSql>

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
    setup();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::setup(){
    if(checkCredentials())
        if(setupSql()){
            uisetup(getRole());
            if(getRole()=="lekarz") this->uid=get_uid();
        }
        else QMessageBox::critical(this,ERROR,ERROR_LOGIN_FAILED,QMessageBox::Ok);
    else QMessageBox::critical(this,ERROR,ENTER_CREDENTIALS,QMessageBox::Ok);
}

bool MainWindow::checkCredentials(){
    if(st.value("sql/hostname").toString()==""||st.value("sql/username").toString()==""||st.value("sql/password").toString()=="") return false;
    return true;
}

void MainWindow::on_Ustawienia_clicked(){
    ustawienia *u;
    u=new ustawienia(this);
    u->exec();
    setup();
}

bool MainWindow::setupSql(){
    QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(st.value("sql/hostname").toString());
    db.setDatabaseName(DB_NAME);
    db.setUserName(st.value("sql/username").toString());
    db.setPassword(st.value("sql/password").toString());
    return db.open();
}

QString MainWindow::getRole(){
    QString r="";
    QSqlQuery q;
    q.exec("select rola from uzytkownicy where login=\""+st.value("sql/username").toString()+"\"");
    q.next();
    r=q.value(0).toString();
    return r;
}

void MainWindow::uisetup(QString r){
    ui->lZalogowany->setText(QString(LOGGED_IN_AS)+": "+st.value("sql/username").toString());
    ui->tTab->setTabVisible(0,r=="administrator");
    ui->tTab->setTabVisible(1,r=="lekarz");
}

void MainWindow::on_btnLekarze_clicked(){
    lekarze *l=new lekarze();
    l->show();
}

void MainWindow::on_btnPacjenci_clicked(){
    pacjenci *p=new pacjenci();
    p->show();
}

void MainWindow::on_btnPoradnie_clicked(){
    poradnie *p=new poradnie();
    p->show();
}

void MainWindow::on_btnAplikacja_clicked(){
    ustawienia_aplikacji *u=new ustawienia_aplikacji();
    u->show();
}

void MainWindow::on_btnAdministratorzy_clicked(){
    administratorzy *a=new administratorzy();
    a->show();
}

int MainWindow::get_uid(){
    QSqlQuery q;
    q.exec("select id from lekarze where nazwa='"+st.value("sql/username").toString()+"'");
    q.next();
    return q.value(0).toInt();
}

void MainWindow::on_btnMojeWizyty_clicked(){
    moje_wizyty *w=new moje_wizyty(nullptr,uid);
    w->show();
}

void MainWindow::on_btnMoiPacjenci_clicked(){
    moi_pacjenci *p=new moi_pacjenci(nullptr,uid);
    p->show();
}
