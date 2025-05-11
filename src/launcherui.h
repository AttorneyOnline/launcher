#pragma once

#include <QObject>

namespace Launcher
{
  class LauncherUI : public QObject
  {
    Q_OBJECT
  public:
    explicit LauncherUI(QObject *parent = nullptr);

  signals:
  };
} // namespace Launcher
