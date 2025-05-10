#include "launchercore.h"
#include "settings.h"
#include "webaccess.h"

#include "version.h"

#include <QNetworkAccessManager>

Launcher::LauncherCore::LauncherCore(QNetworkAccessManager *f_net_man, Launcher::Settings *f_settings, QObject *parent)
    : QObject{parent}
    , webaccess(new Launcher::WebAccess(Launcher::userAgent(), f_settings, f_net_man, this))
    , settings{f_settings}
{
  webaccess->fetch(Endpoint::CLIENTVERSION);
  webaccess->fetch(Endpoint::LAUNCHERVERSION);
  webaccess->fetch(Endpoint::MEDIAREPOSITORY);
}
