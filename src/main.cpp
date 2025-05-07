#include <QGuiApplication>

#include "launchercore.h"
#include "settings.h"
#include "version.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  app.setApplicationDisplayName(Launcher::name());
  app.setApplicationVersion(Launcher::version());

  Launcher::Settings settings(Launcher::organisation(), Launcher::name(), &app);

  LauncherCore launcher(&settings, &app);

  return app.exec();
}
