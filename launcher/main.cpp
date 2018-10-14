#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Attorney Online");
    QCoreApplication::setOrganizationDomain("aceattorneyonline.com");
    QCoreApplication::setApplicationName("Attorney Online Launcher");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    const QSettings settings;
    qDebug() << "Settings:" << settings.fileName();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
