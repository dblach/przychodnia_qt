#include "tworzenie_wizyty.h"
#include "ui_tworzenie_wizyty.h"
#include "strings.h"

tworzenie_wizyty::tworzenie_wizyty(QWidget *parent,int idl,int idp,QDate dt):QDialog(parent),ui(new Ui::tworzenie_wizyty){
    ui->setupUi(this);
    ui->data->setDate(dt);
    setup(idl,idp);
}

tworzenie_wizyty::~tworzenie_wizyty(){
    delete ui;
}

void tworzenie_wizyty::setup(int idp,int idl){
    QSqlQuery q;
    QStringList l;
    l<<"";
    q.exec("select nazwisko,imie,id from pacjenci order by nazwisko");
    while(q.next()) l<<q.value(0).toString()+" "+q.value(1).toString()+" ["+q.value(2).toString()+"]";
    ui->pacjent->addItems(l);
    if(idp){
        for(int i=0;i<ui->pacjent->count();i++) if(ui->pacjent->itemText(i).contains("["+QString::number(idp)+"]")) ui->pacjent->setCurrentIndex(i);
        ui->pacjent->setEnabled(0);
    }

    wczytaj_lekarzy();
    if(idl){
        for(int i=0;i<ui->lekarz->count();i++) if(ui->lekarz->itemText(i).contains("["+QString::number(idl)+"]")) ui->lekarz->setCurrentIndex(i);
    }
    wczytaj_poradnie();
}

void tworzenie_wizyty::wczytaj_lekarzy(){
    QSqlQuery q;
    QStringList l;
    l<<QString(SELECT)+"...";
    q.exec("select nazwisko,imie,id from lekarze order by nazwisko");
    while(q.next()) l<<q.value(0).toString()+" "+q.value(1).toString()+" ["+q.value(2).toString()+"]";
    ui->lekarz->clear();
    ui->lekarz->addItems(l);
}

void tworzenie_wizyty::wczytaj_poradnie(){
   QSqlQuery q;
   QStringList l;
   l<<QString(SELECT)+"...";
   q.exec("select nazwa,id from specjalizacje");
   while(q.next()) l<<q.value(0).toString()+" ["+q.value(1).toString()+"]";
   ui->poradnia->clear();
   ui->poradnia->addItems(l);
}


void tworzenie_wizyty::on_poradnia_currentTextChanged(const QString &t){
   // poradniaChanged(t);
}

void tworzenie_wizyty::on_poradnia_currentIndexChanged(const QString &t){
    if(t.contains("[")) poradniaChanged(t);
}

void tworzenie_wizyty::on_lekarz_currentTextChanged(const QString &t){
  //  lekarzChanged(t);
}

void tworzenie_wizyty::on_lekarz_currentIndexChanged(const QString &t){
    if(t.contains("[")) lekarzChanged(t);
}

void tworzenie_wizyty::lekarzChanged(QString t){
    if(!t.contains("[")) return;
    bool c=false;
    for(int i=1;i<ui->lekarz->count();i++) if(!c) c=t.contains(ui->lekarz->itemText(i));
    if(c) wczytaj_grafik(2);
    lekarz_id=ui->lekarz->currentText().split("[")[1].split("]")[0].toInt();
}

void tworzenie_wizyty::poradniaChanged(QString t){
    bool c=false;
    for(int i=1;i<ui->poradnia->count();i++) if(!c) c=t.contains(ui->poradnia->itemText(i));
    if(c) wczytaj_grafik(1);
}

void tworzenie_wizyty::wczytaj_grafik(int r){
    QSqlQuery q;
    QVector<QMap<QString,QString>> d;
    QVector<QString> queries;
    QString id;
    QStringList h;
    h<<"";
    QString dt=ui->data->date().toString(df);

    if(r==1){ // poradnia
        id=ui->poradnia->currentText().split("[")[1].split("]")[0];
        QSqlQuery q1;
        queries.append("select godzina_rozpoczecia,godzina_zakonczenia,concat_ws(' ',nazwisko,imie),id_lekarza from godziny_przyjec join lekarze on id=id_lekarza where dzien_tygodnia=dayofweek('"+dt+"')-1 and godziny_przyjec.specjalizacja="+id);
        q1.exec("select godzina_rozpoczecia,godzina_zakonczenia,id_lekarza from godziny_przyjec where specjalizacja="+id+" and dzien_tygodnia=dayofweek('"+dt+"')-1");
        while(q1.next()) queries.append("select czas_rozpoczecia,czas_zakonczenia,concat_ws(' ',nazwisko,imie),lekarz_id from wizyty join lekarze on lekarz_id=lekarze.id where data='"+dt+"' and lekarz_id="+q1.value(2).toString()+" and odwolana=0 and czas_rozpoczecia>='"+q1.value(0).toString()+"' and czas_zakonczenia<='"+q1.value(1).toString()+"'");
    }
    if(r==2){ // lekarz
        id=ui->lekarz->currentText().split("[")[1].split("]")[0];
        // zapytanie 1 = godziny przyjec lekarza
        queries.append("select godzina_rozpoczecia,godzina_zakonczenia,nazwa from godziny_przyjec join specjalizacje on specjalizacja=id where id_lekarza="+id+" and dzien_tygodnia=dayofweek('"+dt+"')-1");
        // zapytanie 2 = zajete terminy
        queries.append("select czas_rozpoczecia,czas_zakonczenia,nazwa from wizyty join godziny_przyjec on dzien_tygodnia=dayofweek('"+dt+"')-1 and czas_rozpoczecia>=godzina_rozpoczecia and czas_zakonczenia<=godzina_zakonczenia join specjalizacje on specjalizacja=specjalizacje.id where odwolana=0 and data='"+dt+"' and lekarz_id="+id);
    }

    for(int i=0;i<queries.length();i++){
        q.exec(queries[i]);
        while(q.next()){
            QMap<QString,QString> a;
            a["godzina_rozpoczecia"]=q.value(0).toString();
            a["godzina_zakonczenia"]=q.value(1).toString();
            a["header"]=q.value(2).toString()+(r==1?" ["+q.value(3).toString()+"]":"");
            a["color"]=i>0?"busy":"free";
            if(!i&&!h.contains(a["header"])) h<<a["header"];
            d.append(a);
        }
    }

    ui->tGrafik->setColumnCount(0);
    ui->tGrafik->setColumnCount(h.length());

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
    ui->tGrafik->setRowCount(0);
    ui->tGrafik->setRowCount(c?c:1);
    if(!d.length()){
        QTableWidgetItem *ti=new QTableWidgetItem();
        ti->setText(NOT_FOUND);
        ui->tGrafik->setItem(0,0,ti);
        return;
    }
    t=p;
    for(int i=0;i<c;i++){
        QTableWidgetItem *ti=new QTableWidgetItem();
        ti->setText(t.toString("hh:mm"));
        ui->tGrafik->setItem(i,0,ti);
        t=t.addSecs(st.value("ui/timetable_step").toInt());
    }
    ui->tGrafik->setHorizontalHeaderLabels(h);
    ui->tGrafik->horizontalHeader()->setVisible(1);

    for(int dt=1;dt<h.length();dt++){
        QVectorIterator<QMap<QString,QString>> di(d);
        while(di.hasNext()){
            QMap<QString,QString> it=di.next();
            if(it["header"]!=h[dt]) continue;
            QTime gp=QTime::fromString(it["godzina_rozpoczecia"],"hh:mm:ss");
            QTime gk=QTime::fromString(it["godzina_zakonczenia"],"hh:mm:ss");

            for(int y=0;y<ui->tGrafik->rowCount();++y){
                QTime gc=QTime::fromString(ui->tGrafik->item(y,0)->text(),"hh:mm");
                if(gc>=gp&&gc<gk){
                    if(ui->tGrafik->itemAt(y,dt)){
                        QTableWidgetItem *a=new QTableWidgetItem();
                        ui->tGrafik->setItem(y,dt,a);
                    }
                    ui->tGrafik->item(y,dt)->setBackground(QColor((
                        it["color"]=="busy"?st.value("ui/kolor_zajety_tlo"):
                        it["color"]=="free"?st.value("ui/kolor_wolny_tlo")
                        :st.value("ui/kolor_zwykly_tlo")
                    ).toString()));
                    ui->tGrafik->item(y,dt)->setForeground(QColor((
                        it["color"]=="busy"?st.value("ui/kolor_zajety_tekst"):
                        it["color"]=="free"?st.value("ui/kolor_wolny_tekst")
                        :st.value("ui/kolor_zwykly_tekst")
                    ).toString()));
                }
            }
        }
    }
}

void tworzenie_wizyty::on_rbDoLekarza_clicked(){
    ui->poradnia->setEnabled(0);
    ui->lekarz->setEnabled(1);
}

void tworzenie_wizyty::on_rbDoPoradni_clicked(){
    ui->poradnia->setEnabled(1);
    ui->lekarz->setEnabled(0);
}

void tworzenie_wizyty::on_data_dateChanged(){
    if(ui->rbDoLekarza->isChecked()||ui->rbDoPoradni->isChecked()) wczytaj_grafik(ui->rbDoLekarza->isChecked()?2:1);
}

void tworzenie_wizyty::on_tGrafik_cellDoubleClicked(int r,int c){
    ui->godzina_rozpoczecia->setTime(QTime::fromString(ui->tGrafik->item(r,0)->text(),tf));
    ui->godzina_zakonczenia->setTime(ui->godzina_rozpoczecia->time().addSecs(1800));
    if(ui->rbDoPoradni->isChecked())
        lekarz_id=ui->tGrafik->horizontalHeaderItem(c)->text().split("[")[1].split("]")[0].toInt();
}

void tworzenie_wizyty::on_btnAnuluj_clicked(){
    this->accept();
}

void tworzenie_wizyty::on_btnZapisz_clicked(){
    if(!lekarz_id){
       QMessageBox::critical(this,ERROR,SELECT_DOCTOR,QMessageBox::Ok);
       return;
    }
    if(!ui->pacjent->currentIndex()){
       QMessageBox::critical(this,ERROR,SELECT_PATIENT,QMessageBox::Ok);
       return;
    }
    if(ui->godzina_rozpoczecia->time()>=ui->godzina_zakonczenia->time()){
        QMessageBox::critical(this,ERROR,ENDING_CANNOT_BE_EARLIER_THAN_BEGINNING,QMessageBox::Ok);
        return;
    }
    if(!czyLekarzPrzyjmuje()){
        QMessageBox::critical(this,ERROR,DOCTOR_IS_NOT_AVAILABLE_ON_SELECTED_HOUR,QMessageBox::Ok);
        return;
    }
    if(!czyTerminWolny()){
        QMessageBox::critical(this,ERROR,APPOINTMENT_UNAVAILABLE,QMessageBox::Ok);
        return;
    }
    QSqlQuery q;
    QString pid=ui->pacjent->currentText().split("[")[1].split("]")[0];
    q.exec("insert into wizyty(id,pacjent_id,lekarz_id,data,czas_rozpoczecia,czas_zakonczenia,odwolana) values('',"+pid+","+QString::number(lekarz_id)+",'"+ui->data->date().toString(df)+"','"+ui->godzina_rozpoczecia->time().toString(tf)+"','"+ui->godzina_zakonczenia->time().toString(tf)+"',0)");
    if(q.numRowsAffected()==1) this->accept();
    else QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
}

bool tworzenie_wizyty::czyLekarzPrzyjmuje(){
    QSqlQuery q;
    q.exec("select * from godziny_przyjec where dzien_tygodnia=dayofweek('"+ui->data->date().toString(df)+"')-1 and id_lekarza="+QString::number(lekarz_id)+" and godzina_rozpoczecia<='"+ui->godzina_rozpoczecia->time().toString(tf)+"' and godzina_zakonczenia>='"+ui->godzina_zakonczenia->time().toString(tf)+"'");
    return (q.size())>0;
}

bool tworzenie_wizyty::czyTerminWolny(){
    QSqlQuery q;
    QString wf="HH:mm:ss";
    QTime tp=ui->godzina_rozpoczecia->time();
    QTime tk=ui->godzina_zakonczenia->time();
    q.exec("select czas_rozpoczecia,czas_zakonczenia from wizyty where odwolana=0 and data='"+ui->data->date().toString(df)+"' and lekarz_id="+QString::number(lekarz_id));
    while(q.next()){
        QTime wp=QTime::fromString(q.value(0).toString(),wf);
        QTime wk=QTime::fromString(q.value(1).toString(),wf);
        if(((tk>wp&&tp<=wp)||(wk>tp&&tp>wp))) return false;
    }
    return true;
}
