#include "uploader.h"
#include "ui_uploader.h"

uploader::uploader(QWidget *parent,QString file,QString name_old,QString folder):QDialog(parent),ui(new Ui::uploader){
    ui->setupUi(this);
    QHttpMultiPart *mp=new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QMap<QString,QString> m;
    QMap<QString,QString>::iterator mi;

    QPixmap p(file);
    QImage im=p.toImage();
    QByteArray ba;
    QBuffer bf(&ba);
    im.save(&bf,"JPEG");

    m["username"]=st.value("sql/username").toString();
    m["password"]=st.value("sql/password").toString();
    m["name_old"]=name_old;
    m["folder"]=folder;
    m["data"]=QString::fromLatin1(ba.toBase64().data());

    for(mi=m.begin();mi!=m.end();++mi){
       QHttpPart t;
       t.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data;name=\""+mi.key()+"\""));
       t.setBody(mi.value().toUtf8());
       mp->append(t);
    }

    QNetworkRequest rq(url);
    QNetworkAccessManager *nm=new QNetworkAccessManager(this);
    connect(nm,SIGNAL(finished(QNetworkReply*)),this,SLOT(onReply(QNetworkReply*)));
    QNetworkReply *r=nm->post(rq,mp);
    mp->setParent(r);
}

uploader::~uploader(){
    delete ui;
}

void uploader::onReply(QNetworkReply* r){
    file_new=r->readAll();
    this->accept();
}

QString uploader::getFilename(){
    return file_new;
}
