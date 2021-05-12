#include "moje_wizyty.h"
#include "ui_moje_wizyty.h"
#include "kalendarz.h"
#include "edycja_wizyty.h"
#include "globals.h"

moje_wizyty::moje_wizyty(QWidget *parent,int uid):QWidget(parent),ui(new Ui::moje_wizyty){
    ui->setupUi(this);
    this->id=uid;
    zmien_date(0);
}

moje_wizyty::~moje_wizyty(){
    delete ui;
}

void moje_wizyty::zmien_date(int n){
    ui->btnData->setText((n?QDate::fromString(ui->btnData->text(),DATE_FORMAT).addDays(n):QDate().currentDate()).toString(DATE_FORMAT));
    wczytaj_wizyty();
}
void moje_wizyty::on_btnPoprzedni_clicked(){
    zmien_date(-1);
}

void moje_wizyty::on_btnNastepny_clicked(){
    zmien_date(1);
}

void moje_wizyty::on_btnData_clicked(){
    kalendarz k;
    if(k.exec()==QDialog::Accepted) ui->btnData->setText(k.getDate().toString(DATE_FORMAT));
    wczytaj_wizyty();
}

void moje_wizyty::wczytaj_wizyty(){
    QSqlQuery q;
    QVector<QString> queries;
    QStringList h;
    h<<"";
    QString dt=QDate::fromString(ui->btnData->text(),DATE_FORMAT).toString(DATE_FORMAT_SQL);
    d.clear();

    // zapytanie 1 = godziny przyjec lekarza
    queries.append("select godzina_rozpoczecia,godzina_zakonczenia,nazwa from godziny_przyjec join specjalizacje on specjalizacja=id where id_lekarza="+QString::number(id)+" and dzien_tygodnia=dayofweek('"+dt+"')-1");
    // zapytanie 2 = zajete terminy
    queries.append("select czas_rozpoczecia,czas_zakonczenia,nazwa,concat_ws(' ',pacjenci.nazwisko,pacjenci.imie),wizyty.id from wizyty join godziny_przyjec on dzien_tygodnia=dayofweek('"+dt+"')-1 and czas_rozpoczecia>=godzina_rozpoczecia and czas_zakonczenia<=godzina_zakonczenia join specjalizacje on specjalizacja=specjalizacje.id join pacjenci on pacjent_id=pacjenci.id where odwolana=0 and data='"+dt+"' and lekarz_id="+QString::number(id));

    for(int i=0;i<queries.length();i++){
        q.exec(queries[i]);
        while(q.next()){
            QMap<QString,QString> a;
            a["godzina_rozpoczecia"]=q.value(0).toString();
            a["godzina_zakonczenia"]=q.value(1).toString();
            a["header"]=q.value(2).toString();
            a["title"]=q.value(3).toString();
            a["id"]=q.value(4).toString();
            a["color"]=i>0?"busy":"free";
            if(!i&&!h.contains(a["header"])) h<<a["header"];
            d.append(a);
        }
    }

    ui->tWizyty->setColumnCount(0);
    ui->tWizyty->setColumnCount(h.length());

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
        ti->setText("Nie znaleziono");
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
    ui->tWizyty->setHorizontalHeaderLabels(h);
    ui->tWizyty->horizontalHeader()->setVisible(1);

    for(int dt=1;dt<h.length();dt++){
        QVectorIterator<QMap<QString,QString>> di(d);
        while(di.hasNext()){
            QMap<QString,QString> it=di.next();
            if(it["header"]!=h[dt]) continue;
            QTime gp=QTime::fromString(it["godzina_rozpoczecia"],"hh:mm:ss");
            QTime gk=QTime::fromString(it["godzina_zakonczenia"],"hh:mm:ss");
            QString t=it["title"];
            for(int y=0;y<ui->tWizyty->rowCount();++y){
                QTime gc=QTime::fromString(ui->tWizyty->item(y,0)->text(),"hh:mm");
                if(gc>=gp&&gc<gk){
                    if(ui->tWizyty->itemAt(y,dt)){
                        QTableWidgetItem *a=new QTableWidgetItem();
                        a->setText(t);
                        t="";
                        ui->tWizyty->setItem(y,dt,a);
                    }
                    ui->tWizyty->item(y,dt)->setBackground(QColor((
                        it["color"]=="busy"?st.value("ui/kolor_zajety_tlo"):
                        it["color"]=="free"?st.value("ui/kolor_wolny_tlo")
                        :st.value("ui/kolor_zwykly_tlo")
                    ).toString()));
                    ui->tWizyty->item(y,dt)->setForeground(QColor((
                        it["color"]=="busy"?st.value("ui/kolor_zajety_tekst"):
                        it["color"]=="free"?st.value("ui/kolor_wolny_tekst")
                        :st.value("ui/kolor_zwykly_tekst")
                    ).toString()));
                }
            }
        }
    }
}

void moje_wizyty::on_btnOdswiez_clicked(){
    wczytaj_wizyty();
}

void moje_wizyty::on_tWizyty_cellDoubleClicked(int r,int dt){
    if(!dt) return;
    int idp=0;
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
        while(di.hasNext()&&!idp){
            QMap<QString,QString> it=di.next();
            if(QTime::fromString(it["godzina_rozpoczecia"],"hh:mm:ss")>=p && QTime::fromString(it["godzina_zakonczenia"],"hh:mm:ss")<=k && ui->tWizyty->horizontalHeaderItem(dt)->text()==it["header"] && it["id"]!="" )
                idp=it["id"].toInt();
        }
    }
    else{
        gp=QTime::fromString(ui->tWizyty->item(r,0)->text(),"hh:mm");
    }

    if(!idp) return;
    edycja_wizyty *e;
    e=new edycja_wizyty(this,idp);
    if(e->exec()==QDialog::Accepted) wczytaj_wizyty();
}
