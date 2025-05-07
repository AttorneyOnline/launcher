#include "launchercore.h"
#include "settings.h"

#include <QNetworkAccessManager>

LauncherCore::LauncherCore(Launcher::Settings *f_settings, QObject *parent)
    : QObject{parent}
    , net_man{new QNetworkAccessManager(this)}
    , settings{}
{}
