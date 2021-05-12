#include "edycja_pacjenta.h"
#include "ui_edycja_pacjenta.h"
#include "tworzenie_wizyty.h"
#include "edycja_wizyty.h"
#include "kalendarz.h"
#include "globals.h"
#include "strings.h"
#include <QtSql>
#include <QMessageBox>
#include <QDate>

edycja_pacjenta::edycja_pacjenta(QWidget *parent,int id,bool allow_editing):QDialog(parent),ui(new Ui::edycja_pacjenta){
    ui->setupUi(this);
    if(!id){
        ui->btnUsun->setEnabled(0);
    }
    else{
        ui->login->setEnabled(0);
        ui->haslo->setEnabled(0);
        load_data(id);
    }
    if(!allow_editing){
        ui->imie->setEnabled(0);
        ui->nazwisko->setEnabled(0);
        ui->adres_ulica->setEnabled(0);
        ui->adres_kod->setEnabled(0);
        ui->adres_miasto->setEnabled(0);
        ui->telefon->setEnabled(0);
        ui->btnUsun->setEnabled(0);
        ui->tabWidget->setTabVisible(1,0);
    }
    zmien_tydzien(0);
}

edycja_pacjenta::~edycja_pacjenta(){
    delete ui;
}

void edycja_pacjenta::load_data(int id){
    QSqlQuery q;
    q.exec("select imie,nazwisko,login,adres_ulica,adres_kod,adres_miasto,telefon,notatki from pacjenci where id="+QString::number(id));
    q.next();
    ui->id->setText(QString::number(id));
    ui->imie->setText(q.value(0).toString());
    ui->nazwisko->setText(q.value(1).toString());
    ui->login->setText(q.value(2).toString());
    ui->adres_ulica->setText(q.value(3).toString());
    ui->adres_kod->setText(q.value(4).toString());
    ui->adres_miasto->setText(q.value(5).toString());
    ui->telefon->setText(q.value(6).toString());
    ui->notatki->setText(q.value(7).toString());
}

void edycja_pacjenta::on_btnZapisz_clicked(){
    if(
        ui->imie->text()==""
     || ui->nazwisko->text()==""
     || ui->login->text()==""
    ){
        QMessageBox::critical(this,ERROR,ENTER_DATA,QMessageBox::Ok);
        return;
    }
    QSqlQuery q;
    QString id=ui->id->text();
    QString imie=ui->imie->text();
    QString nazwisko=ui->nazwisko->text();
    QString login=ui->login->text();
    QString haslo=ui->haslo->text();
    QString adres_ulica=ui->adres_ulica->text();
    QString adres_kod=ui->adres_kod->text().replace("-","");
    QString adres_miasto=ui->adres_miasto->text();
    QString telefon=ui->telefon->text();
    QString notatki=ui->notatki->toPlainText();

    //dodanie konta uzytkownika
    if(id==""){
        q.exec("insert into uzytkownicy(id,login,rola) values('','"+login+"','pacjent')");
        if(q.numRowsAffected()!=1){
            QMessageBox::critical(this,ERROR,ERROR_LOGIN_IN_USE,QMessageBox::Ok);
            return;
        }
        q.exec("create user "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".godziny_przyjec to "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".konfiguracja to "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".lekarze to "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".pacjenci to "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".specjalizacje to "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".wiadomosci to "+login+" identified by '"+haslo+"'");
        q.exec("grant select,insert,update on "+QString(DB_NAME)+".wizyty to "+login+" identified by '"+haslo+"'");
    }

    q.exec(id==""?
           "insert into pacjenci(id,imie,nazwisko,login,adres_ulica,adres_kod,adres_miasto,telefon,notatki) values('','"+imie+"','"+nazwisko+"','"+login+"','"+adres_ulica+"','"+adres_kod+"','"+adres_miasto+"','"+telefon+"','"+notatki+"')"
           :"update pacjenci set imie='"+imie+"',nazwisko='"+nazwisko+"',adres_ulica='"+adres_ulica+"',adres_kod='"+adres_kod+"',adres_miasto='"+adres_miasto+"',telefon='"+telefon+"',notatki='"+notatki+"' where id="+id
    );
    if(q.numRowsAffected()!=1){
        QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
        return;
    }

    this->accept();
}

void edycja_pacjenta::on_btnUsun_clicked(){
    if(QMessageBox::question(this,CONFIRM,CONFIRM_PATIENT_DELETION,QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) return;
    QSqlQuery q;
    q.exec("drop user "+ui->login->text());
    q.exec("delete from pacjenci where id="+ui->id->text());
    q.exec("delete from wizyty where pacjent_id="+ui->id->text());
    q.exec("delete from uzytkownicy where login='"+ui->login->text()+"'");
    if(q.numRowsAffected()==1)
        this->accept();
    else
        QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
}

void edycja_pacjenta::on_btnAnuluj_clicked(){
    this->reject();
}

void edycja_pacjenta::on_tabWidget_currentChanged(int index){
    if(index==1) zmien_tydzien(0);
}

void edycja_pacjenta::wczytaj_tydzien(){
    QString dt[8]={"",WEEKDAY_MON,WEEKDAY_TUE,WEEKDAY_WED,WEEKDAY_THU,WEEKDAY_FRI,WEEKDAY_SAT,WEEKDAY_SUN};
    QDate dp=QDate::fromString(ui->btnData->text().split(" - ")[0],df);
    l.clear();
    l<<"";
    for(int i=1;i<8;i++) l<<dt[i]+dp.addDays(i).toString("dd.MM");
    ui->tWizyty->setRowCount(0);
    ui->tWizyty->setColumnCount(8);
    ui->tWizyty->horizontalHeader()->setVisible(true);
    ui->tWizyty->setHorizontalHeaderLabels(l);

    QSqlQuery q;
    q.exec("select data,czas_rozpoczecia,czas_zakonczenia,imie,nazwisko,wizyty.id from wizyty join lekarze on lekarze.id=lekarz_id where odwolana=0 and pacjent_id="+ui->id->text()+" and data>='"+dp.toString("yyyy-MM-dd")+"' and data<='"+dp.addDays(7).toString("yyyy-MM-dd")+"'");
    while(q.next()){
        QMap<QString,QString> a;
        a["data"]=QDate::fromString(q.value(0).toString(),"yyyy-MM-dd").toString("dd.MM");
        a["godzina_rozpoczecia"]=q.value(1).toString();
        a["godzina_zakonczenia"]=q.value(2).toString();
        a["lekarz"]=q.value(4).toString()+" "+q.value(3).toString();
        a["id"]=q.value(5).toString();
        d.append(a);
    }

    QTime p(23,59,59);
    QTime k(0,0,0);
    QVectorIterator<QMap<QString,QString>> di(d);
    while(di.hasNext()){
        QMap<QString,QString> it=di.next();
        QTime gp=QTime::fromString(it["godzina_rozpoczecia"],"hh:mm:ss");
        QTime gk=QTime::fromString(it["godzina_zakonczenia"],"hh:mm:ss");
        if(gp<p) p=gp;
        if(gk>k) k=gk;
    }
    QTime t=p;
    int c=0;
    while(t<k){
        c++;
        t=t.addSecs(st.value("ui/timetable_step").toInt());
    }
    ui->tWizyty->setRowCount(0);
    ui->tWizyty->setRowCount(c?c:1);
    if(!d.length()){
        QTableWidgetItem *ti=new QTableWidgetItem();
        ti->setText(NOT_FOUND);
        ui->tWizyty->setItem(0,0,ti);
        return;
    }
    t=p;
    for(int i=0;i<c;i++){
        QTableWidgetItem *ti=new QTableWidgetItem();
        ti->setText(t.toString("hh:mm"));
        ui->tWizyty->setItem(i,0,ti);
        t=t.addSecs(st.value("ui/timetable_step").toInt());
    }

    for(int dt=1;dt<8;dt++){
        QVectorIterator<QMap<QString,QString>> di(d);
        while(di.hasNext()){
            QMap<QString,QString> it=di.next();
            if(!l[dt].contains(it["data"])) continue;
            QTime gp=QTime::fromString(it["godzina_rozpoczecia"],"hh:mm:ss");
            QTime gk=QTime::fromString(it["godzina_zakonczenia"],"hh:mm:ss");
            QString lk=it["lekarz"];
            for(int y=0;y<ui->tWizyty->rowCount();++y){
                QTime gc=QTime::fromString(ui->tWizyty->item(y,0)->text(),"hh:mm");
                if(gc>=gp&&gc<gk){
                    QTableWidgetItem *a=new QTableWidgetItem();
                    a->setText(lk);
                    lk="";
                    ui->tWizyty->setItem(y,dt,a);
                    ui->tWizyty->item(y,dt)->setBackground(QColor(st.value("ui/kolor_zwykly_tlo").toString()));
                    ui->tWizyty->item(y,dt)->setForeground(QColor(st.value("ui/kolor_zwykly_tekst").toString()));
                }
            }
        }
    }
}

void edycja_pacjenta::on_btnPoprzedni_clicked(){
    zmien_tydzien(-1);
}


void edycja_pacjenta::on_btnNastepny_clicked(){
    zmien_tydzien(1);
}

void edycja_pacjenta::zmien_tydzien(int t){
    QDate d=t?
        QDate::fromString(ui->btnData->text().split(" ")[0],df).addDays(7*t):
        QDate::currentDate().addDays(-QDate::currentDate().dayOfWeek()+1);
    ui->btnData->setText(d.toString(df)+" - "+d.addDays(6).toString(df));
    wczytaj_tydzien();
}

void edycja_pacjenta::on_tWizyty_cellDoubleClicked(int r,int dt){
    if(!dt) return;
    int idw=0;
    QTime gp(0,0,0);
    QTime p;
    QTime k;
    if(ui->tWizyty->item(r,dt)){
        int y=r;
        if(ui->tWizyty->item(y,dt)&&ui->tWizyty->item(y,dt)->text()!=""){
            p=QTime::fromString(ui->tWizyty->item(y,0)->text(),"hh:mm");
        }
        else{
            while(1){
                if(ui->tWizyty->item(y,dt)&&ui->tWizyty->item(y,dt)->text()!="") p=QTime::fromString(ui->tWizyty->item(y,0)->text(),"hh:mm"); else break;
                y--;
            }
        }
        y=r++;
        k=p.addSecs(st.value("ui/timetable_step").toInt());
        while(1){
            if(ui->tWizyty->item(y,dt)&&ui->tWizyty->item(y,dt)->text()=="") k=QTime::fromString(ui->tWizyty->item(y,0)->text(),"hh:mm").addSecs(st.value("ui/timetable_step").toInt());
            y++;
            if(!ui->tWizyty->item(y,dt)||ui->tWizyty->item(y,dt)->text()!="") break;
        }

        QVectorIterator<QMap<QString,QString>> di(d);
        while(di.hasNext()&&!idw){
            QMap<QString,QString> it=di.next();
            if(!l[dt].contains(it["data"])) continue;
            if(QTime::fromString(it["godzina_rozpoczecia"],"hh:mm:ss")>=p && QTime::fromString(it["godzina_zakonczenia"],"hh:mm:ss")<=k)
                idw=it["id"].toInt();
        }
    }
    else{
        gp=QTime::fromString(ui->tWizyty->item(r,0)->text(),"hh:mm");
    }
    if(!idw) return;

    edycja_wizyty *e=new edycja_wizyty(this,idw);
    e->exec();
    wczytaj_tydzien();
}

void edycja_pacjenta::on_btnDodajWizyte_clicked(){
    tworzenie_wizyty *t=new tworzenie_wizyty(this,ui->id->text().toInt(),0);
    t->exec();
    wczytaj_tydzien();
}

void edycja_pacjenta::on_btnData_clicked(){
    kalendarz k;
    if(k.exec()==QDialog::Accepted){
        QDate d=k.getDate();
        d=d.addDays(-d.dayOfWeek()+1);
        ui->btnData->setText(d.toString(df)+" - "+d.addDays(6).toString(df));
        wczytaj_tydzien();
    }
}
