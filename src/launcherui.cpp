#include "launcherui.h"

#include <QQmlApplicationEngine>
#include <QQmlComponent>

using namespace Launcher;

LauncherUI::LauncherUI(QObject *parent)
    : QObject{parent}
{
  QQmlEngine engine(this);
  QQmlComponent component(&engine);
}
