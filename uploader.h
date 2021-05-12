#ifndef UPLOADER_H
#define UPLOADER_H

#include <QDialog>
#include <QSettings>
#include <QUrl>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QBuffer>
#include "globals.h"

namespace Ui {
class uploader;
}

class uploader : public QDialog
{
    Q_OBJECT

public:
    explicit uploader(QWidget *parent = nullptr,QString file="",QString name_old="",QString folder="");
    ~uploader();
    QString getFilename();

private slots:
    void onReply(QNetworkReply*);

private:
    Ui::uploader *ui;
    QSettings st;
    QUrl url=QUrl(NETWORK_PROTOCOL+st.value("sql/hostname").toString()+"/interface/upload.php");
    QString file_new="";
};

#endif // UPLOADER_H
