#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Devesh Rai");
    QCoreApplication::setOrganizationDomain("drai.in");
    QCoreApplication::setApplicationName("Easy Picker");
    MainWindow w;
    w.show();
    return a.exec();
}
