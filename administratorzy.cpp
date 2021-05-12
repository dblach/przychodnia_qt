#include "administratorzy.h"
#include "ui_administratorzy.h"
#include "edycja_administratora.h"

administratorzy::administratorzy(QWidget *parent):QWidget(parent),ui(new Ui::administratorzy){
    ui->setupUi(this);
    get();
}

administratorzy::~administratorzy(){
    delete ui;
}

void administratorzy::get(){
    QSqlQuery q;
    QStringList l;
    l<<"ID"<<"Login";
    ui->tAdministratorzy->setHorizontalHeaderLabels(l);
    ui->tAdministratorzy->setRowCount(0);

    q.exec("select id,login from uzytkownicy where rola='administrator'");
    while(q.next()){
        int r=ui->tAdministratorzy->rowCount();
        ui->tAdministratorzy->insertRow(r);
        for(int i=0;i<2;i++){
            QTableWidgetItem* ti=new QTableWidgetItem();
            ti->setText(q.value(i).toString());
            ui->tAdministratorzy->setItem(r,i,ti);
        }
    }
}

void administratorzy::on_tAdministratorzy_cellDoubleClicked(int r){
    otworz_edytor(ui->tAdministratorzy->item(r,0)->text().toInt());
}

void administratorzy::on_btnDodaj_clicked(){
    otworz_edytor(0);
}

void administratorzy::otworz_edytor(int id){
    edycja_administratora *e;
    e=new edycja_administratora(this,id);
    if(e->exec()==QDialog::Accepted) get();
}
