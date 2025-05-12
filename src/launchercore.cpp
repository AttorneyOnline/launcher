#include "launchercore.h"
#include "launcherui.h"
#include "settings.h"
#include "webaccess.h"

#include "version.h"

#include <QNetworkAccessManager>

Launcher::LauncherCore::LauncherCore(QNetworkAccessManager *f_net_man, Settings *f_settings, QObject *parent)
    : QObject{parent}
    , webaccess(new WebAccess(Launcher::userAgent(), f_settings, f_net_man, this))
    , settings{f_settings}
    , user_interface(new LauncherUI(this))
{
  webaccess->fetch(Endpoint::CLIENTVERSION);
  webaccess->fetch(Endpoint::LAUNCHERVERSION);
  webaccess->fetch(Endpoint::MEDIAREPOSITORY);
}
