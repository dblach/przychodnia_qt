#include "pacjenci.h"
#include "ui_pacjenci.h"
#include "edycja_pacjenta.h"
#include "strings.h"
#include <QtSql>
#include <QDebug>

pacjenci::pacjenci(QWidget *parent):QWidget(parent),ui(new Ui::pacjenci){
    ui->setupUi(this);
    setup();
}

pacjenci::~pacjenci(){
    delete ui;
}

void pacjenci::setup(){
    QStringList l;
    l<<ID<<SURNAME<<NAME<<ADDRESS<<POSTCODE<<CITY;
    ui->tPacjenci->setHorizontalHeaderLabels(l);
    ui->tPacjenci->setRowCount(0);

    QSqlQuery q;
    q.exec("select id,nazwisko,imie,adres_ulica,adres_kod,adres_miasto from pacjenci");
    while(q.next()){
        int r=ui->tPacjenci->rowCount();
        ui->tPacjenci->insertRow(r);
        for(int i=0;i<6;i++){
            QTableWidgetItem* ti=new QTableWidgetItem();
            ti->setText(q.value(i).toString());
            ui->tPacjenci->setItem(r,i,ti);
        }
    }
}

void pacjenci::on_filtr_textChanged(const QString &arg1){
    for(int i=0;i<ui->tPacjenci->rowCount();i++){
        bool match=false;
        for(int j=0;j<ui->tPacjenci->columnCount();j++){
            match=ui->tPacjenci->item(i,j)->text().toLower().contains(arg1.toLower());
            if(match) break;
        }
        ui->tPacjenci->setRowHidden(i,!match);
    }
}

void pacjenci::on_btnCzysc_clicked(){
    ui->filtr->setText("");
}

void pacjenci::on_btnNowy_clicked(){
    edycja_pacjenta *e;
    e=new edycja_pacjenta(this,0);
    if(e->exec()==QDialog::Accepted) setup();
}

void pacjenci::on_tPacjenci_cellDoubleClicked(int r){
    edycja_pacjenta *e;
    e=new edycja_pacjenta(this,ui->tPacjenci->item(r,0)->text().toInt());
    if(e->exec()==QDialog::Accepted) setup();
}
