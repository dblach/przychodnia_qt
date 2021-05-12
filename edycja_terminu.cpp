#include "edycja_terminu.h"
#include "ui_edycja_terminu.h"
#include "strings.h"
#include <QtSql>
#include <QMessageBox>

edycja_terminu::edycja_terminu(QWidget *parent,int idt,int idl,int dt,QTime gp):QDialog(parent),ui(new Ui::edycja_terminu){
    ui->setupUi(this);
    getLekarze();
    getSpecjalizacje();
    ui->id->setText(QString::number(idt));
    QStringList dni;
    dni<<WEEKDAY_MONDAY<<WEEKDAY_TUESDAY<<WEEKDAY_WEDNESDAY<<WEEKDAY_THURSDAY<<WEEKDAY_FRIDAY<<WEEKDAY_SATURDAY<<WEEKDAY_SUNDAY;
    ui->dzien->addItems(dni);
    if(idl)
        for(int i=0;i<ui->lekarz->count();i++)
            if(ui->lekarz->itemText(i).contains(" ["+QString::number(idl)+"]"))
                ui->lekarz->setCurrentIndex(i);
    if(idt){
        ui->btnUsun->setEnabled(1);
        setup(idt);
        return;
    }
    if(dt){
        ui->dzien->setCurrentIndex(dt-1);
        ui->gp->setTime(gp);
        ui->gk->setTime(gp.addSecs(1800));
    }
}

edycja_terminu::~edycja_terminu(){
    delete ui;
}

void edycja_terminu::setup(int idt){
    QSqlQuery q;
    q.exec("select specjalizacja,dzien_tygodnia,godzina_rozpoczecia,godzina_zakonczenia,pomieszczenie from godziny_przyjec where id_przyjecia="+QString::number(idt));
    q.next();
    for(int i=0;i<ui->poradnia->count();i++)
        if(ui->poradnia->itemText(i).contains(" ["+q.value(0).toString()+"]"))
            ui->poradnia->setCurrentIndex(i);
    ui->dzien->setCurrentIndex(q.value(1).toInt()-1);
    ui->gp->setTime(QTime::fromString(q.value(2).toString(),"hh:mm:ss"));
    ui->gk->setTime(QTime::fromString(q.value(3).toString(),"hh:mm:ss"));
    ui->pomieszczenie->setText(q.value(4).toString());
}

void edycja_terminu::getLekarze(){
    QSqlQuery q;
    QStringList l;
    q.exec("select nazwisko,imie,id from lekarze order by nazwisko");
    while(q.next()) l<<q.value(0).toString()+" "+q.value(1).toString()+" ["+q.value(2).toString()+"]";
    ui->lekarz->addItems(l);
}

void edycja_terminu::getSpecjalizacje(){
    QSqlQuery q;
    QStringList l;
    q.exec("select nazwa,id from specjalizacje");
    while(q.next()) l<<q.value(0).toString()+" ["+q.value(1).toString()+"]";
    ui->poradnia->addItems(l);
}

void edycja_terminu::on_btnAnuluj_clicked(){
    this->accept();
}

void edycja_terminu::on_btnOk_clicked(){
    QString idp=ui->id->text()=="0"?"":ui->id->text();
    QString idl=ui->lekarz->currentText().split(" [")[1].split("]")[0];
    QString dzien=QString::number(ui->dzien->currentIndex()+1);
    QString poradnia=ui->poradnia->currentText().split(" [")[1].split("]")[0];
    QString pomieszczenie=ui->pomieszczenie->text();
    QString gp=ui->gp->time().toString("hh:mm");
    QString gk=ui->gk->time().toString("hh:mm");

    if(ui->gp->time()>=ui->gk->time()){
        QMessageBox::critical(this,ERROR,ENDING_CANNOT_BE_EARLIER_THAN_BEGINNING,QMessageBox::Ok);
        return;
    }
    if(sprawdzTermin(idl,dzien,gp,gk)>(idp==""?0:1)){
        QMessageBox::critical(this,ERROR,APPOINTMENT_UNAVAILABLE,QMessageBox::Ok);
        return;
    }

    QSqlQuery q;
    q.exec(idp==""?
           "insert into godziny_przyjec(id_przyjecia,id_lekarza,specjalizacja,dzien_tygodnia,godzina_rozpoczecia,godzina_zakonczenia,pomieszczenie) values('',"+idl+","+poradnia+","+dzien+",'"+gp+"','"+gk+"','"+pomieszczenie+"')"
          :"update godziny_przyjec set id_lekarza="+idl+",specjalizacja="+poradnia+",dzien_tygodnia="+dzien+",godzina_rozpoczecia='"+gp+"',godzina_zakonczenia='"+gk+"',pomieszczenie='"+pomieszczenie+"' where id_przyjecia="+idp
    );
    if(q.numRowsAffected()!=1)
        QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
    else
        this->accept();
}

int edycja_terminu::sprawdzTermin(QString idl,QString dzien,QString gp,QString gk){
    QSqlQuery q;
    q.exec("select id_przyjecia from godziny_przyjec where id_lekarza="+idl+" and dzien_tygodnia="+dzien+" and (godzina_rozpoczecia>='"+gp+"' and godzina_zakonczenia<='"+gk+"')");
    return q.size();
}

void edycja_terminu::on_btnUsun_clicked(){
    if(QMessageBox::question(this,"Potwierdź","Czy na pewno chcesz usunąć?",QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) return;
    QSqlQuery q;
    q.exec("delete from godziny_przyjec where id_przyjecia="+ui->id->text());
    if(q.numRowsAffected()==1)
        this->accept();
    else
        QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
}
