#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Attorney Online");
    QCoreApplication::setOrganizationDomain("aceattorneyonline.com");
    QCoreApplication::setApplicationName("Attorney Online Launcher");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::applicationDirPath());

    const QSettings settings;
    qDebug() << "Settings:" << settings.fileName();

    MainWindow w;
    w.show();

    return a.exec();
}
