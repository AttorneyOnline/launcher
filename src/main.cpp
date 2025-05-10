#include <QGuiApplication>

#include "launchercore.h"
#include "settings.h"
#include "version.h"

#include <QNetworkAccessManager>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  app.setApplicationDisplayName(Launcher::name());
  app.setApplicationVersion(Launcher::version());

  QNetworkAccessManager net_man(&app);
  Launcher::Settings settings(Launcher::organisation(), Launcher::name(), &app);
  Launcher::LauncherCore launcher(&net_man, &settings, &app);

  return app.exec();
}
