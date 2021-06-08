#include "edycja_aktualnosci.h"
#include "ui_edycja_aktualnosci.h"
#include "uploader.h"
#include "globals.h"
#include "strings.h"

edycja_aktualnosci::edycja_aktualnosci(QWidget *parent,int id):QDialog(parent),ui(new Ui::edycja_aktualnosci){
    ui->setupUi(this);
    this->id=id;
    ui->data_dodania->setText(QDate::currentDate().toString("yyyy-MM-dd")+" "+QTime::currentTime().toString("HH:mm"));
    if(!id) ui->btnUsun->setVisible(0);
    else{
        QSqlQuery q;
        q.exec("select data_dodania,nazwa,tresc,obraz from wiadomosci where id="+QString::number(id));
        q.next();
        ui->data_dodania->setText(q.value(0).toString().replace("T"," ").replace(".000",""));
        ui->tytul->setText(q.value(1).toString());
        ui->tresc->insertPlainText(q.value(2).toString());
        zdj=q.value(3).toString();

        if(zdj=="") return;
        QNetworkAccessManager *n=new QNetworkAccessManager(this);
        connect(n,&QNetworkAccessManager::finished,this,&edycja_aktualnosci::setImage);
        QNetworkRequest rq(NETWORK_PROTOCOL+st.value("sql/hostname").toString()+"/interface/images/"+zdj);
        n->get(rq);
    }
}

edycja_aktualnosci::~edycja_aktualnosci(){
    delete ui;
}

void edycja_aktualnosci::setImage(QNetworkReply *r){
    QPixmap p;
    p.loadFromData(r->readAll());
    ui->zdjecie->setPixmap(p.scaled(ui->zdjecie->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void edycja_aktualnosci::on_btnZmienZdjecie_clicked(){
    zdj_n=QFileDialog::getOpenFileName(this,IMAGE_DIALOG_TITLE,IMAGE_DIALOG_DEFAULT_FOLDER,IMAGE_DIALOG_FILEMASK);
    if(zdj_n=="") return;
    QPixmap p(zdj_n);
    ui->zdjecie->setPixmap(p.scaled(ui->zdjecie->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    zdj_zmienione=true;
}

void edycja_aktualnosci::on_btnAnuluj_clicked(){
    this->reject();
}


void edycja_aktualnosci::on_btnZapisz_clicked(){
    if(ui->tytul->text()==""||ui->tresc->toPlainText()==""){
        QMessageBox::critical(this,ERROR,ENTER_DATA,QMessageBox::Ok);
        return;
    }
    else{
        if(zdj_zmienione){
            uploader u(this,zdj_n,zdj,"images");
            if(u.exec()==QDialog::Accepted) zdj=u.getFilename();
            else{
                QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPLOAD_PHOTO,QMessageBox::Ok);
                return;
            }
        }
        QSqlQuery q;
        q.exec(id?
                   "update wiadomosci set nazwa='"+ui->tytul->text()+"',obraz='"+zdj+"',tresc='"+ui->tresc->toPlainText().replace("'","\\""")+"' where id="+QString::number(id)
                  :"insert into wiadomosci(id,nazwa,tresc,obraz) values('','"+ui->tytul->text()+"','"+ui->tresc->toPlainText().replace("'","\\""")+"','"+zdj+"')"
        );
        if(q.numRowsAffected()==1) this->accept();
        else
            QMessageBox::information(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
    }
}

void edycja_aktualnosci::on_btnUsun_clicked(){
    if(QMessageBox::question(this,CONFIRM,CONFIRM_DELETION,QMessageBox::Yes|QMessageBox::No)==QMessageBox::No) return;
    QSqlQuery q;
    q.exec("delete from wiadomosci where id="+QString::number(id));
    if(q.numRowsAffected()==1) this->accept();
    else QMessageBox::critical(this,ERROR,ERROR_CANNOT_UPDATE,QMessageBox::Ok);
}
