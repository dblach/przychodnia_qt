#include "edycja_wizyty.h"
#include "ui_edycja_wizyty.h"
#include "strings.h"

edycja_wizyty::edycja_wizyty(QWidget *parent,int id):QDialog(parent),ui(new Ui::edycja_wizyty){
    ui->setupUi(this);
    this->id=id;
    QSqlQuery q;
    q.exec("select data,czas_rozpoczecia,czas_zakonczenia,notatka,lekarz_id,concat_ws(' ',lekarze.nazwisko,lekarze.imie),pacjent_id,concat_ws(' ',pacjenci.nazwisko,pacjenci.imie) from wizyty join lekarze on lekarze.id=lekarz_id join pacjenci on pacjenci.id=pacjent_id where wizyty.id="+QString::number(id));
    if(!q.next()){
        QMessageBox::critical(this,ERROR,CANNOT_RETRIEVE_APPOINTMENT_DATA,QMessageBox::Ok);
        return;
    }
    ui->data->setDate(QDate::fromString(q.value(0).toString(),df));
    ui->godzina_rozpoczecia->setTime(QTime::fromString(q.value(1).toString(),tf));
    ui->godzina_zakonczenia->setTime(QTime::fromString(q.value(2).toString(),tf));
    ui->notatka->setText(q.value(3).toString());
    ui->lekarz->setText(q.value(5).toString()+" ["+q.value(4).toString()+"]");
    ui->pacjent->setText(q.value(7).toString()+" ["+q.value(6).toString()+"]");
}

edycja_wizyty::~edycja_wizyty(){
    delete ui;
}

void edycja_wizyty::on_btnAnuluj_clicked(){
    this->reject();
}

void edycja_wizyty::on_btnOk_clicked(){
    QSqlQuery q;
    q.exec("update wizyty set notatka='"+ui->notatka->toPlainText()+"' where id="+QString::number(this->id));
    if(q.numRowsAffected()==1)
        this->accept();
    else
        QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
}

void edycja_wizyty::on_btnOdwolaj_clicked(){
    if(QMessageBox::question(this,CONFIRM,CONFIRM_APPOINTMENT_DELETION,QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) return;
    QSqlQuery q;
    q.exec("update wizyty set odwolana=1 where id="+QString::number(this->id));
    if(q.numRowsAffected()==1)
        this->accept();
    else
        QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
}
