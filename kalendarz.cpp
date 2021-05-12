#include "kalendarz.h"
#include "ui_kalendarz.h"

kalendarz::kalendarz(QWidget *parent):QDialog(parent),ui(new Ui::kalendarz){
    ui->setupUi(this);
}

kalendarz::~kalendarz(){
    delete ui;
}

void kalendarz::on_btnAnuluj_clicked(){
    this->reject();
}

void kalendarz::on_btnOk_clicked(){
    this->accept();
}

QDate kalendarz::getDate(){
    return ui->cal->selectedDate();
}
