#include <QGuiApplication>

#include "launchercore.h"
#include "version.h"
#include "settings.h"

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);
  app.setApplicationDisplayName(Launcher::name());
  app.setApplicationVersion(Launcher::version());

  Launcher::Settings settings(Launcher::organisation(), Launcher::name(), &app);

  LauncherCore launcher(&settings, &app);

  return app.exec();
}
