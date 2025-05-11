#pragma once

#include <QObject>
#include <qqmlintegration.h>

namespace Launcher
{
  class LauncherQMLInterface : public QObject
  {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Because I say so.")

  public:
    LauncherQMLInterface();
  };
} // namespace Launcher
