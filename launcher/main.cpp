#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QResource>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Attorney Online");
    QCoreApplication::setOrganizationDomain("aceattorneyonline.com");
    QCoreApplication::setApplicationName("Attorney Online Launcher");
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::applicationDirPath());
    QDir::setSearchPaths("branding", {":/branding", ":/res"});

    const QSettings settings;
    qDebug() << "Settings:" << settings.fileName();

    MainWindow w;
    w.show();
    w.refresh();

    return a.exec();
}
