#include "ustawienia.h"
#include "ui_ustawienia.h"
#include "strings.h"

ustawienia::ustawienia(QWidget *parent):QDialog(parent),ui(new Ui::ustawienia){
    ui->setupUi(this);
    getSettings();
}

ustawienia::~ustawienia(){
    delete ui;
}

void ustawienia::getSettings(){
    ui->sql_hostname->setText(s.value("sql/hostname").toString());
    ui->sql_username->setText(s.value("sql/username").toString());
    ui->sql_password->setText(s.value("sql/password").toString());
    int v;
    if((v=s.value("ui/timetable_step").toInt()/60)) ui->timetable_step->setValue(v);
    ui->btnZwykly_tekst->setStyleSheet("background-color:"+s.value("ui/kolor_zwykly_tekst").toString()+";");
    ui->btnZwykly_tlo->setStyleSheet("background-color:"+s.value("ui/kolor_zwykly_tlo").toString()+";");
    ui->btnWolny_tekst->setStyleSheet("background-color:"+s.value("ui/kolor_wolny_tekst").toString()+";");
    ui->btnWolny_tlo->setStyleSheet("background-color:"+s.value("ui/kolor_wolny_tlo").toString()+";");
    ui->btnZajety_tekst->setStyleSheet("background-color:"+s.value("ui/kolor_zajety_tekst").toString()+";");
    ui->btnZajety_tlo->setStyleSheet("background-color:"+s.value("ui/kolor_zajety_tlo").toString()+";");
}

void ustawienia::saveSettings(){
    s.setValue("sql/hostname",ui->sql_hostname->text());
    s.setValue("sql/username",ui->sql_username->text());
    s.setValue("sql/password",ui->sql_password->text());
    s.setValue("ui/timetable_step",ui->timetable_step->value()*60);
    s.setValue("ui/kolor_zwykly_tekst",ui->btnZwykly_tekst->styleSheet().replace("background-color:","").replace(";",""));
    s.setValue("ui/kolor_zwykly_tlo",ui->btnZwykly_tlo->styleSheet().replace("background-color:","").replace(";",""));
    s.setValue("ui/kolor_wolny_tekst",ui->btnWolny_tekst->styleSheet().replace("background-color:","").replace(";",""));
    s.setValue("ui/kolor_wolny_tlo",ui->btnWolny_tlo->styleSheet().replace("background-color:","").replace(";",""));
    s.setValue("ui/kolor_zajety_tekst",ui->btnZajety_tekst->styleSheet().replace("background-color:","").replace(";",""));
    s.setValue("ui/kolor_zajety_tlo",ui->btnZajety_tlo->styleSheet().replace("background-color:","").replace(";",""));
    s.sync();
    this->accept();
}

void ustawienia::on_btnOk_clicked(){
    saveSettings();
}

QString ustawienia::ustaw_kolor(){
    QColor c=QColorDialog::getColor(Qt::white,this,SELECT_COLOR);
    return c.isValid()?"background-color:"+c.name()+";":"";
}

void ustawienia::on_btnZwykly_tekst_clicked(){
    ui->btnZwykly_tekst->setStyleSheet(ustaw_kolor());
}

void ustawienia::on_btnZwykly_tlo_clicked(){
    ui->btnZwykly_tlo->setStyleSheet(ustaw_kolor());
}

void ustawienia::on_btnWolny_tekst_clicked(){
    ui->btnWolny_tekst->setStyleSheet(ustaw_kolor());
}

void ustawienia::on_btnWolny_tlo_clicked(){
    ui->btnWolny_tlo->setStyleSheet(ustaw_kolor());
}

void ustawienia::on_btnZajety_tekst_clicked(){
    ui->btnZajety_tekst->setStyleSheet(ustaw_kolor());
}

void ustawienia::on_btnZajety_tlo_clicked(){
    ui->btnZajety_tlo->setStyleSheet(ustaw_kolor());
}

void ustawienia::on_btnCancel_clicked(){
    this->reject();
}
