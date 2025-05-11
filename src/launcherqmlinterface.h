#pragma once

#include <QObject>
#include <qqmlintegration.h>

namespace Launcher
{
  class LauncherQMLInterface : public QObject
  {
    Q_OBJECT
    QML_ELEMENT

  public:
    LauncherQMLInterface();
  };
} // namespace Launcher
