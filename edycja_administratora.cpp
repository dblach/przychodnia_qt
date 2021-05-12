#include "edycja_administratora.h"
#include "ui_edycja_administratora.h"
#include "strings.h"
#include "globals.h"

edycja_administratora::edycja_administratora(QWidget *parent,int id):QDialog(parent),ui(new Ui::edycja_administratora){
    ui->setupUi(this);
    this->id=id;
    if(id){
        QSqlQuery q;
        q.exec("select login from uzytkownicy where id="+QString::number(id));
        q.next();
        ui->login->setText(q.value(0).toString());
        ui->login->setEnabled(0);
        ui->haslo->setEnabled(0);
        ui->btnZapisz->setEnabled(0);
    }
    else{
        ui->btnUsun->setVisible(0);
    }
}

edycja_administratora::~edycja_administratora(){
    delete ui;
}

void edycja_administratora::on_btnUsun_clicked(){
    if(QMessageBox::question(this,CONFIRM,CONFIRM_DELETION,QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) return;
    QSqlQuery q;
    q.exec("drop user "+ui->login->text());
    q.exec("delete from uzytkownicy where login='"+ui->login->text()+"'");
    if(q.numRowsAffected()==1) this->accept();
    else QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
}

void edycja_administratora::on_btnZapisz_clicked(){
    if(ui->login->text()==""||(!id&&ui->haslo->text()=="")){
        QMessageBox::critical(this,ERROR,ENTER_DATA,QMessageBox::Ok);
        return;
    }
    QSqlQuery q;
    QString ids=QString::number(id);
    QString login=ui->login->text();
    QString haslo=ui->haslo->text();
    if(!id){
        q.exec("insert into uzytkownicy(id,login,rola) values('','"+login+"','administrator')");
        if(q.numRowsAffected()!=1){
            QMessageBox::critical(this,ERROR,ERROR_LOGIN_IN_USE,QMessageBox::Ok);
            return;
        }
        q.exec("create user "+login+" identified by '"+haslo+"'");
        q.exec("grant all privileges on "+QString(DB_NAME)+".* to "+login+" identified by '"+haslo+"'");
        }
   this->accept();
}



void edycja_administratora::on_btnAnuluj_clicked(){
    this->reject();
}
