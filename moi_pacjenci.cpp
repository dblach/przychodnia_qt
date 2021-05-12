#include "moi_pacjenci.h"
#include "ui_moi_pacjenci.h"
#include "strings.h"
#include "edycja_pacjenta.h"

moi_pacjenci::moi_pacjenci(QWidget *parent,int idl):QWidget(parent),ui(new Ui::moi_pacjenci){
    ui->setupUi(this);
    this->idl=idl;
    setup();
}

moi_pacjenci::~moi_pacjenci(){
    delete ui;
}

void moi_pacjenci::setup(){
    QStringList l;
    l<<ID<<SURNAME<<NAME<<ADDRESS<<POSTCODE<<CITY;
    ui->tPacjenci->setHorizontalHeaderLabels(l);
    ui->tPacjenci->setRowCount(0);

    QSqlQuery q;
    QSqlQuery q1;
    q.exec("select distinct pacjent_id from wizyty where lekarz_id="+QString::number(idl));
    while(q.next()){
        q1.exec("select id,nazwisko,imie,adres_ulica,adres_kod,adres_miasto from pacjenci where id="+q.value(0).toString());
        q1.next();
        int r=ui->tPacjenci->rowCount();
        ui->tPacjenci->insertRow(r);
        for(int i=0;i<6;i++){
            QTableWidgetItem* ti=new QTableWidgetItem();
            ti->setText(q1.value(i).toString());
            ui->tPacjenci->setItem(r,i,ti);
        }
    }
}

void moi_pacjenci::on_btnOdswiez_clicked(){
    setup();
}

void moi_pacjenci::on_tPacjenci_cellDoubleClicked(int r){
    edycja_pacjenta *e;
    e=new edycja_pacjenta(this,ui->tPacjenci->item(r,0)->text().toInt(),false);
    if(e->exec()==QDialog::Accepted) setup();
}

void moi_pacjenci::on_filtr_textChanged(const QString &arg1){
    for(int i=0;i<ui->tPacjenci->rowCount();i++){
        bool match=false;
        for(int j=0;j<ui->tPacjenci->columnCount();j++){
            match=ui->tPacjenci->item(i,j)->text().toLower().contains(arg1.toLower());
            if(match) break;
        }
        ui->tPacjenci->setRowHidden(i,!match);
    }
}

void moi_pacjenci::on_btnCzysc_clicked(){
    ui->filtr->setText("");
}
