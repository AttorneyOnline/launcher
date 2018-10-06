#include "mainwindow.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Attorney Online");
    QCoreApplication::setOrganizationDomain("aceattorneyonline.com");
    QCoreApplication::setApplicationName("Attorney Online Launcher");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
