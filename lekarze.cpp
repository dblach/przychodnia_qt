#include "lekarze.h"
#include "ui_lekarze.h"
#include "edycja_lekarza.h"
#include "strings.h"
#include <QtSql>
#include <QVector>
#include <QMap>
#include <QTime>

lekarze::lekarze(QWidget *parent):QWidget(parent),ui(new Ui::lekarze){
    ui->setupUi(this);
    setupWgNazwisk();
}

lekarze::~lekarze(){
    delete ui;
}

void lekarze::on_tabWidget_currentChanged(int index){
    switch(index){
        case 0:setupWgNazwisk(); break;
        case 1:setupWgGodzinPrzyjec(); break;
        default:break;
    }
}

void lekarze::setupWgNazwisk(){
    QStringList l;
    l<<ID<<SURNAME<<NAME;
    ui->tWgNazwisk_table->setHorizontalHeaderLabels(l);
    ui->tWgNazwisk_table->setRowCount(0);

    QSqlQuery q;
    q.exec("select id,nazwisko,imie from lekarze order by nazwisko");
    while(q.next()){
        int r=ui->tWgNazwisk_table->rowCount();
        ui->tWgNazwisk_table->insertRow(r);
        for(int i=0;i<3;i++){
            QTableWidgetItem* ti=new QTableWidgetItem();
            ti->setText(q.value(i).toString());
            ui->tWgNazwisk_table->setItem(r,i,ti);
        }
    }
}

void lekarze::on_tWgNazwisk_filtr_textChanged(){
    tWgNazwisk_filtruj();
}

void lekarze::tWgNazwisk_filtruj(){
    QString f=ui->tWgNazwisk_filtr->text().toLower();
    for(int i=0;i<ui->tWgNazwisk_table->rowCount();i++){
        bool match=false;
        for(int j=0;j<ui->tWgNazwisk_table->columnCount();j++){
            match=ui->tWgNazwisk_table->item(i,j)->text().toLower().contains(f);
            if(match) break;
        }
        ui->tWgNazwisk_table->setRowHidden(i,!match);
    }
}

void lekarze::on_tWgNazwisk_filtr_czysc_clicked(){
    ui->tWgNazwisk_filtr->setText("");
}

void lekarze::setupWgGodzinPrzyjec(){
    QSqlQuery q;
    QMap<int,QMap<QString,QString>> d;
    QMap<int,QMap<QString,QString>>::iterator di;
    QMap<QString,QString> lekarze;
    QTime p(23,59,59);
    QTime k(0,0,0);
    int i=-1;

    q.exec("select id_przyjecia,imie,nazwisko,godzina_rozpoczecia,godzina_zakonczenia,specjalizacje.nazwa,id_lekarza from godziny_przyjec join lekarze on godziny_przyjec.id_lekarza=lekarze.id join specjalizacje on godziny_przyjec.specjalizacja=specjalizacje.id where dzien_tygodnia="+QString::number(ui->tWgGodzinPrzyjec_dzien->currentIndex()+1));
    while(q.next()){
        QMap<QString,QString> a;
        a["lekarz"]=q.value(2).toString()+" "+q.value(1).toString();
        a["id_lekarza"]=q.value(6).toString();
        a["godzina_rozpoczecia"]=q.value(3).toString();
        a["godzina_zakonczenia"]=q.value(4).toString();
        a["specjalizacja"]=q.value(5).toString();
        d[i++]=a;
    }

    for(di=d.begin();di!=d.end();++di){
        QTime gp=QTime::fromString(di.value()["godzina_rozpoczecia"],"hh:mm:ss");
        QTime gk=QTime::fromString(di.value()["godzina_zakonczenia"],"hh:mm:ss");
        if(gp<p) p=gp;
        if(gk>k) k=gk;
    }

    ui->tWgGodzinPrzyjec_table->setRowCount(0);
    if(p>k) return;

    QTime t=p;
    int c=0;
    while(t<k){
        c++;
        t=t.addSecs(st.value("ui/timetable_step").toInt());
    }
    ui->tWgGodzinPrzyjec_table->setRowCount(c);
    t=p;
    for(int i=0;i<c;i++){
        QTableWidgetItem *ti=new QTableWidgetItem();
        ti->setText(t.toString("hh:mm"));
        ui->tWgGodzinPrzyjec_table->setItem(i,0,ti);
        t=t.addSecs(st.value("ui/timetable_step").toInt());
    }
    ui->tWgGodzinPrzyjec_table->horizontalHeader()->setVisible(true);
    QStringList l;
    l<<"";
    for(di=d.begin();di!=d.end();++di){
        QString lek=di.value()["id_lekarza"];
        if(!l.contains(lek)){
            l<<lek;
            lekarze[lek]=di.value()["lekarz"];
        }
    }
    QStringList ll;
    for(const auto& id:l) ll<<lekarze[id];
    ui->tWgGodzinPrzyjec_table->setColumnCount(ll.count());
    ui->tWgGodzinPrzyjec_table->setHorizontalHeaderLabels(ll);

    int x=0;
    for(const auto& lek:l){
        for(di=d.begin();di!=d.end();++di){
            if(di.value()["id_lekarza"]!=lek) continue;
            QTime gp=QTime::fromString(di.value()["godzina_rozpoczecia"],"hh:mm:ss");
            QTime gk=QTime::fromString(di.value()["godzina_zakonczenia"],"hh:mm:ss");
            QString sp=di.value()["specjalizacja"];
            for(int y=0;y<ui->tWgGodzinPrzyjec_table->rowCount();++y){
                QTime gc=QTime::fromString(ui->tWgGodzinPrzyjec_table->item(y,0)->text(),"hh:mm");
                if(gc>=gp&&gc<=gk){
                    QTableWidgetItem *a=new QTableWidgetItem();
                    a->setText(sp);
                    sp="";
                    ui->tWgGodzinPrzyjec_table->setItem(y,x,a);
                    ui->tWgGodzinPrzyjec_table->item(y,x)->setBackground(QColor(st.value("ui/kolor_zwykly_tlo").toString()));
                    ui->tWgGodzinPrzyjec_table->item(y,x)->setForeground(QColor(st.value("ui/kolor_zwykly_tekst").toString()));
                }
            }
        }
        x++;
    }
}

void lekarze::on_tWgGodzinPrzyjec_dzien_currentIndexChanged(){
    setupWgGodzinPrzyjec();
}

void lekarze::on_tWgNazwisk_btnNowy_clicked(){
    edycja_lekarza *e;
    e=new edycja_lekarza(this,0);
    if(e->exec()==QDialog::Accepted) setupWgNazwisk();
}

void lekarze::on_tWgNazwisk_table_cellDoubleClicked(int r){
    edycja_lekarza *e;
    e=new edycja_lekarza(this,ui->tWgNazwisk_table->item(r,0)->text().toInt());
    if(e->exec()==QDialog::Accepted) setupWgNazwisk();
}
