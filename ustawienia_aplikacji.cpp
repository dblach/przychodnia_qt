#include "ustawienia_aplikacji.h"
#include "ui_ustawienia_aplikacji.h"
#include "edycja_aktualnosci.h"
#include "uploader.h"
#include "globals.h"
#include "strings.h"

ustawienia_aplikacji::ustawienia_aplikacji(QWidget *parent):QWidget(parent),ui(new Ui::ustawienia_aplikacji){
    ui->setupUi(this);
    ui->btnDodaj->setVisible(0);
    pobierz_ustawienia();
}

ustawienia_aplikacji::~ustawienia_aplikacji(){
    delete ui;
}

QString ustawienia_aplikacji::get(QString n){
    QSqlQuery q;
    q.exec("select wartosc from konfiguracja where ustawienie='"+n+"'");
    q.next();
    return q.value(0).toString();
}

bool ustawienia_aplikacji::set(QString k,QString v){
    QSqlQuery q;
    q.exec("update konfiguracja set wartosc='"+v+"' where ustawienie='"+k+"'");
    return q.lastError().text()=="";
}

void ustawienia_aplikacji::pobierz_ustawienia(){
    ui->tekst_powitalny->setText(get("tekst_powitalny"));
    ui->czas_na_edycje->setValue(get("czas_na_edycje").toInt());
    logo=get("logo");
    if(logo=="") return;
    QNetworkAccessManager *n=new QNetworkAccessManager(this);
    connect(n,&QNetworkAccessManager::finished,this,&ustawienia_aplikacji::setImage);
    QNetworkRequest rq(NETWORK_PROTOCOL+st.value("sql/hostname").toString()+"/interface/images/"+logo);
    n->get(rq);
}

void ustawienia_aplikacji::setImage(QNetworkReply *r){
    QPixmap p;
    p.loadFromData(r->readAll());
    ui->logo->setPixmap(p.scaled(ui->logo->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void ustawienia_aplikacji::on_btnAnuluj_clicked(){
    delete this;
}

void ustawienia_aplikacji::on_btnZapisz_clicked(){
    if(logo_zmienione){
        uploader u(this,logo_n,logo,"images");
        if(u.exec()==QDialog::Accepted) logo=u.getFilename();
        else{
            QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPLOAD_PHOTO,QMessageBox::Ok);
            return;
        }
    }

    QMap<QString,QString> u;
    QMap<QString,QString>::iterator it;

    u["tekst_powitalny"]=ui->tekst_powitalny->text();
    u["logo"]=logo;
    u["czas_na_edycje"]=QString::number(ui->czas_na_edycje->value());

    for(it=u.begin();it!=u.end();++it)
       if(!set(it.key(),it.value())){
           QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
           return;
       }
    delete this;
}

void ustawienia_aplikacji::on_btnZmienLogo_clicked(){
    logo_n=QFileDialog::getOpenFileName(this,IMAGE_DIALOG_TITLE,".",IMAGE_DIALOG_FILEMASK);
    if(logo_n=="") return;
    QPixmap p(logo_n);
    ui->logo->setPixmap(p.scaled(ui->logo->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    logo_zmienione=true;
}

void ustawienia_aplikacji::on_tabWidget_currentChanged(int index){
    ui->btnDodaj->setVisible(index);
    if(index==1) wczytaj_aktualnosci();
}

void ustawienia_aplikacji::wczytaj_aktualnosci(){
    QSqlQuery q;
    QStringList l;
    l<<ID<<DATE_ADDED<<HEADER<<CONTENT;
    ui->tAktualnosci->setHorizontalHeaderLabels(l);
    ui->tAktualnosci->setRowCount(0);

    q.exec("select id,data_dodania,nazwa,concat(substring(tresc,1,40),'...') from wiadomosci order by data_dodania desc");
    while(q.next()){
        int r=ui->tAktualnosci->rowCount();
        ui->tAktualnosci->insertRow(r);
        for(int i=0;i<4;i++){
            QTableWidgetItem* ti=new QTableWidgetItem();
            QString v=q.value(i).toString();
            if(i==1) v=v.replace("T"," ").replace(".000","");
            ti->setText(v);
            ui->tAktualnosci->setItem(r,i,ti);
        }
    }
}

void ustawienia_aplikacji::otworz_edytor(int id){
    edycja_aktualnosci *e;
    e=new edycja_aktualnosci(this,id);
    if(e->exec()==QDialog::Accepted) wczytaj_aktualnosci();
}

void ustawienia_aplikacji::on_tAktualnosci_cellDoubleClicked(int r){
    otworz_edytor(ui->tAktualnosci->item(r,0)->text().toInt());
}

void ustawienia_aplikacji::on_btnDodaj_clicked(){
    otworz_edytor(0);
}
