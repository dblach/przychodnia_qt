#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("przychodnia_qt");
    a.setOrganizationName("dblach");
    a.setOrganizationDomain("pl");
    MainWindow w;
    w.show();
    return a.exec();
}
