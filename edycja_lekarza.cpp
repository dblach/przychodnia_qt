#include "edycja_lekarza.h"
#include "ui_edycja_lekarza.h"
#include "edycja_terminu.h"
#include "uploader.h"
#include "globals.h"
#include "strings.h"
#include <QtSql>
#include <QMessageBox>
#include <QFileDialog>

edycja_lekarza::edycja_lekarza(QWidget *parent,int id):QDialog(parent),ui(new Ui::edycja_lekarza){
    ui->setupUi(this);
    this->idl=id;
    setup(id);
    if(id) setupGodzinyPrzyjec();
    else ui->tabWidget->removeTab(1);
}

edycja_lekarza::~edycja_lekarza(){
    delete ui;
}

void edycja_lekarza::setup(int id){
    ui->specjalizacja->addItems(pobierz_specjalizacje());
    if(!id){
        ui->btnUsun->setEnabled(0);
        ui->login->setEnabled(1);
        ui->haslo->setEnabled(1);
        ui->btnDodajTermin->setEnabled(0);
        return;
    }
    ui->btnDodajTermin->setVisible(0);
    QSqlQuery q;
    q.exec("select imie,nazwisko,specjalizacja,nazwa,zdjecie from lekarze where id="+QString::number(id));
    q.next();
    ui->id->setText(QString::number(id));
    ui->imie->setText(q.value(0).toString());
    ui->nazwisko->setText(q.value(1).toString());
    ui->login->setText(q.value(3).toString());    
    zdj=q.value(4).toString();
    for(int i=0;i<ui->specjalizacja->count();i++)
        if(ui->specjalizacja->itemText(i).contains(" ("+q.value(2).toString()+")"))
            ui->specjalizacja->setCurrentIndex(i);

    QNetworkAccessManager *n=new QNetworkAccessManager(this);
    connect(n,&QNetworkAccessManager::finished,this,&edycja_lekarza::setImage);
    QNetworkRequest rq(NETWORK_PROTOCOL+st.value("sql/hostname").toString()+"/interface/doctor/"+zdj);
    n->get(rq);
}

void edycja_lekarza::setImage(QNetworkReply *r){
    QPixmap p;
    p.loadFromData(r->readAll());
    ui->zdjecie->setPixmap(p.scaled(ui->zdjecie->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

QStringList edycja_lekarza::pobierz_specjalizacje(){
    QStringList s;
    QSqlQuery q;
    q.exec("select nazwa,id from specjalizacje");
    while(q.next()) s<<q.value(0).toString()+" ("+q.value(1).toString()+")";
    ui->specjalizacja->clear();
    return s;
}

void edycja_lekarza::on_btnAnuluj_clicked(){
    this->reject();
}

void edycja_lekarza::on_btnZapisz_clicked(){
    if(
        ui->imie->text()==""
     || ui->nazwisko->text()==""
     || ui->login->text()==""
     || (!idl&&ui->haslo->text()=="")
    ){
        QMessageBox::critical(this,ERROR,ENTER_DATA,QMessageBox::Ok);
        return;
    }
    QSqlQuery q;
    QString imie=ui->imie->text();
    QString nazwisko=ui->nazwisko->text();
    QString login=ui->login->text();
    QString haslo=ui->haslo->text();
    QString specjalizacja=ui->specjalizacja->currentText().split("(")[1].split(")")[0];
    q.exec(!idl?
           "insert into lekarze(id,imie,nazwisko,nazwa,specjalizacja) values('','"+imie+"','"+nazwisko+"','"+login+"',"+specjalizacja+")"
           :"update lekarze set imie='"+imie+"',nazwisko='"+nazwisko+"',specjalizacja="+specjalizacja+" where id="+ui->id->text()
    );
    if(q.numRowsAffected()!=1){
        QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
        return;
    }

    //dodanie konta uzytkownika
    if(!idl){
        q.exec("insert into uzytkownicy(id,login,rola) values('','"+login+"','lekarz')");
        if(q.numRowsAffected()!=1){
            QMessageBox::critical(this,ERROR,ERROR_LOGIN_IN_USE,QMessageBox::Ok);
            return;
        }
        //pobranie id
        //q.exec("select max(id) from lekarze");
        //q.next();
        //idl=q.value(0).toInt();
        q.exec("create user "+login+" identified by '"+haslo+"'");
        q.exec("grant select,insert,update on "+QString(DB_NAME)+".godziny_przyjec to "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".lekarze to "+login+" identified by '"+haslo+"'");
        q.exec("grant select,update on "+QString(DB_NAME)+".pacjenci to "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".specjalizacje to "+login+" identified by '"+haslo+"'");
        q.exec("grant select on "+QString(DB_NAME)+".uzytkownicy to "+login+" identified by '"+haslo+"'");
        q.exec("grant select,insert,update on eskulap.wizyty to "+login+" identified by '"+haslo+"'");

    }

    if(zdj_zmienione){
        uploader u(this,zdj_n,zdj,"doctor");
        if(u.exec()==QDialog::Accepted) zdj=u.getFilename();
        else{
            QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPLOAD_PHOTO,QMessageBox::Ok);
            return;
        }
    }
    q.exec("update lekarze set zdjecie='"+zdj+"' where nazwa='"+login+"'");
    this->accept();
}

void edycja_lekarza::on_btnUsun_clicked(){
    if(QMessageBox::question(this,CONFIRM,CONFIRM_DOCTOR_DELETION,QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) return;
    QSqlQuery q;
    q.exec("drop user "+ui->login->text());
    q.exec("delete from lekarze where id="+ui->id->text());
    q.exec("delete from wizyty where lekarz_id="+ui->id->text());
    q.exec("delete from godziny_przyjec where id_lekarza="+ui->id->text());
    q.exec("delete from uzytkownicy where login='"+ui->login->text()+"'");
    if(q.numRowsAffected()==1)
        this->accept();
    else
        QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
}

void edycja_lekarza::on_btnZmienZdjecie_clicked(){
    zdj_n=QFileDialog::getOpenFileName(this,IMAGE_DIALOG_TITLE,IMAGE_DIALOG_DEFAULT_FOLDER,IMAGE_DIALOG_FILEMASK);
    if(zdj_n=="") return;
    QPixmap p(zdj_n);
    ui->zdjecie->setPixmap(p.scaled(ui->zdjecie->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    zdj_zmienione=true;
}

void edycja_lekarza::on_tabWidget_tabBarClicked(int index){
    if(index==1) setupGodzinyPrzyjec();
}

void edycja_lekarza::setupGodzinyPrzyjec(){
    if(ui->id->text()=="") return;
    d.clear();
    QStringList dni;
    dni<<""<<WEEKDAY_MONDAY<<WEEKDAY_TUESDAY<<WEEKDAY_WEDNESDAY<<WEEKDAY_THURSDAY<<WEEKDAY_FRIDAY<<WEEKDAY_SATURDAY<<WEEKDAY_SUNDAY;
    QSqlQuery q;
    QMap<int,QString> sp;
    while(ui->tGodzinyPrzyjec->rowCount()>0) ui->tGodzinyPrzyjec->removeRow(0);
    ui->tGodzinyPrzyjec->clear();
    ui->tGodzinyPrzyjec->viewport()->update();
    ui->tGodzinyPrzyjec->horizontalHeader()->setVisible(true);
    ui->tGodzinyPrzyjec->setColumnCount(dni.count());
    ui->tGodzinyPrzyjec->setHorizontalHeaderLabels(dni);
    q.exec("select id,nazwa from specjalizacje");
    while(q.next()) sp[q.value(0).toInt()]=q.value(1).toString();

    q.exec("select * from godziny_przyjec where id_lekarza="+ui->id->text());
    while(q.next()){
        QMap<QString,QString> a;
        a["id"]=q.value(0).toString();
        a["specjalizacja_id"]=q.value(2).toString();
        a["specjalizacja_nazwa"]=sp[q.value(2).toInt()];
        a["dzien_tygodnia"]=q.value(3).toString();
        a["godzina_rozpoczecia"]=q.value(4).toString();
        a["godzina_zakonczenia"]=q.value(5).toString();
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
    ui->tGodzinyPrzyjec->setRowCount(c);
    t=p;
    for(int i=0;i<c;i++){
        QTableWidgetItem *ti=new QTableWidgetItem();
        ti->setText(t.toString("hh:mm"));
        ui->tGodzinyPrzyjec->setItem(i,0,ti);
        t=t.addSecs(st.value("ui/timetable_step").toInt());
    }

    for(int dt=1;dt<8;dt++){
        QVectorIterator<QMap<QString,QString>> di(d);
        while(di.hasNext()){
            QMap<QString,QString> it=di.next();
            if(it["dzien_tygodnia"].toInt()!=dt) continue;
            QTime gp=QTime::fromString(it["godzina_rozpoczecia"],"hh:mm:ss");
            QTime gk=QTime::fromString(it["godzina_zakonczenia"],"hh:mm:ss");
            QString sp=it["specjalizacja_nazwa"];
            for(int y=0;y<ui->tGodzinyPrzyjec->rowCount();++y){
                QTime gc=QTime::fromString(ui->tGodzinyPrzyjec->item(y,0)->text(),"hh:mm");
                if(gc>=gp&&gc<gk){
                    QTableWidgetItem *a=new QTableWidgetItem();
                    a->setText(sp);
                    sp="";
                    ui->tGodzinyPrzyjec->setItem(y,dt,a);
                    ui->tGodzinyPrzyjec->item(y,dt)->setBackground(QColor(st.value("ui/kolor_zwykly_tlo").toString()));
                    ui->tGodzinyPrzyjec->item(y,dt)->setForeground(QColor(st.value("ui/kolor_zwykly_tekst").toString()));
                }
            }
        }
    }
}

void edycja_lekarza::on_tGodzinyPrzyjec_cellDoubleClicked(int r,int dt){
    if(!dt) return;
    int idp=0;
    QTime gp(0,0,0);
    QTime p;
    QTime k;
    if(ui->tGodzinyPrzyjec->item(r,dt)){
        int y=r;
        if(ui->tGodzinyPrzyjec->item(y,dt)&&ui->tGodzinyPrzyjec->item(y,dt)->text()!=""){
            p=QTime::fromString(ui->tGodzinyPrzyjec->item(y,0)->text(),"hh:mm");
        }
        else{
            while(1){
                if(ui->tGodzinyPrzyjec->item(y,dt)&&ui->tGodzinyPrzyjec->item(y,dt)->text()!="") p=QTime::fromString(ui->tGodzinyPrzyjec->item(y,0)->text(),"hh:mm"); else break;
                y--;
            }
        }
        y=r++;
        k=p.addSecs(st.value("ui/timetable_step").toInt());
        while(1){
            if(ui->tGodzinyPrzyjec->item(y,dt)&&ui->tGodzinyPrzyjec->item(y,dt)->text()=="") k=QTime::fromString(ui->tGodzinyPrzyjec->item(y,0)->text(),"hh:mm").addSecs(st.value("ui/timetable_step").toInt());
            y++;
            if(!ui->tGodzinyPrzyjec->item(y,dt)||ui->tGodzinyPrzyjec->item(y,dt)->text()!="") break;
        }

        QVectorIterator<QMap<QString,QString>> di(d);
        while(di.hasNext()&&!idp){
            QMap<QString,QString> it=di.next();
            if(it["dzien_tygodnia"].toInt()!=dt) continue;
            if(QTime::fromString(it["godzina_rozpoczecia"],"hh:mm:ss")>=p && QTime::fromString(it["godzina_zakonczenia"],"hh:mm:ss")<=k)
                idp=it["id"].toInt();
        }
    }
    else{
        gp=QTime::fromString(ui->tGodzinyPrzyjec->item(r,0)->text(),"hh:mm");
    }

    edycja_terminu *e;
    e=new edycja_terminu(this,idp,ui->id->text().toInt(),dt,gp);
    if(e->exec()==QDialog::Accepted) setupGodzinyPrzyjec();
}

void edycja_lekarza::on_tabWidget_currentChanged(int index){
    ui->btnDodajTermin->setVisible(index);
}

void edycja_lekarza::on_btnDodajTermin_clicked(){
    edycja_terminu *e;
    e=new edycja_terminu(this,0,ui->id->text().toInt(),0,QTime());
    if(e->exec()==QDialog::Accepted) setupGodzinyPrzyjec();
}
