#include "poradnie.h"
#include "ui_poradnie.h"
#include "edycja_poradni.h"
#include "strings.h"

poradnie::poradnie(QWidget *parent):QWidget(parent),ui(new Ui::poradnie){
    ui->setupUi(this);
    get();
}

poradnie::~poradnie(){
    delete ui;
}

void poradnie::get(){
    QSqlQuery q;
    QStringList h;
    q.exec("select id,nazwa from specjalizacje order by nazwa");
    h<<ID<<TITLE;
    ui->tPoradnie->setColumnCount(2);
    ui->tPoradnie->setRowCount(0);
    ui->tPoradnie->setHorizontalHeaderLabels(h);
    while(q.next()){
        int r=ui->tPoradnie->rowCount();
        ui->tPoradnie->insertRow(r);
        for(int i=0;i<2;i++){
            QTableWidgetItem* ti=new QTableWidgetItem();
            ti->setText(q.value(i).toString());
            ui->tPoradnie->setItem(r,i,ti);
        }
    }
}

void poradnie::on_tPoradnie_cellDoubleClicked(int r){
    otworz_edytor(ui->tPoradnie->item(r,0)->text().toInt());
}

void poradnie::on_btnDodaj_clicked(){
    otworz_edytor(0);
}

void poradnie::otworz_edytor(int i){
    edycja_poradni *e;
    e=new edycja_poradni(this,i);
    if(e->exec()==QDialog::Accepted) get();
}
