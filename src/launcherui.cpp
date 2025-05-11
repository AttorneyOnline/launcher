#include "launcherui.h"

#include <QQmlApplicationEngine>

using namespace Launcher;

LauncherUI::LauncherUI(QObject *parent)
    : QObject{parent}
{
  QQmlEngine engine(this);
  QQmlComponent
}
