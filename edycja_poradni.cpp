#include "edycja_poradni.h"
#include "ui_edycja_poradni.h"
#include "uploader.h"
#include "globals.h"
#include "strings.h"

edycja_poradni::edycja_poradni(QWidget *parent,int idp):QDialog(parent),ui(new Ui::edycja_poradni){
    ui->setupUi(this);
    this->idp=idp;
    setup();
}

edycja_poradni::~edycja_poradni(){
    delete ui;
}

void edycja_poradni::setup(){
    if(!idp){
        ui->tabWidget->removeTab(1);
        return;
    }
    QSqlQuery q;
    q.exec("select nazwa,ikona from specjalizacje where id="+QString::number(this->idp));
    q.next();
    ui->nazwa->setText(q.value(0).toString());
    zdj=q.value(1).toString();
    if(zdj=="") return;

    QNetworkAccessManager *n=new QNetworkAccessManager(this);
    connect(n,&QNetworkAccessManager::finished,this,&edycja_poradni::setImage);
    QNetworkRequest rq(NETWORK_PROTOCOL+st.value("sql/hostname").toString()+"/interface/clinic/"+zdj);
    n->get(rq);
}

void edycja_poradni::setImage(QNetworkReply *r){
    QPixmap p;
    p.loadFromData(r->readAll());
    ui->zdjecie->setPixmap(p.scaled(ui->zdjecie->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void edycja_poradni::on_btnZmienZdjecie_clicked(){
    zdj_n=QFileDialog::getOpenFileName(this,IMAGE_DIALOG_TITLE,".",IMAGE_DIALOG_FILEMASK);
    if(zdj_n=="") return;
    QPixmap p(zdj_n);
    ui->zdjecie->setPixmap(p.scaled(ui->zdjecie->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    zdj_zmienione=true;
}

void edycja_poradni::on_btnZapisz_clicked(){
    if(ui->nazwa->text()==""){
        QMessageBox::critical(this,ERROR,ENTER_NAME,QMessageBox::Ok);
        return;
    }
    if(zdj_zmienione){
        uploader u(this,zdj_n,zdj,"clinic");
        if(u.exec()==QDialog::Accepted){
            zdj=u.getFilename();
        }
        else{
            QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPLOAD_PHOTO,QMessageBox::Ok);
            return;
        }
    }
    QSqlQuery q;
    q.exec(idp?
        "update specjalizacje set nazwa='"+ui->nazwa->text()+"',ikona='"+zdj+"' where id="+QString::number(idp)
       :"insert into specjalizacje(id,nazwa,ikona) values('','"+ui->nazwa->text()+"','"+zdj+"')"

    );
    if(q.numRowsAffected()==1){
        this->accept();
    }
    else{
        QMessageBox::information(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
    }
}

void edycja_poradni::on_btnAnuluj_clicked(){
    this->reject();
}
