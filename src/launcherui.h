#pragma once

#include <QObject>

class QQmlEngine;

namespace Launcher
{
  class LauncherUI : public QObject
  {
    Q_OBJECT
  public:
    explicit LauncherUI(QObject *parent = nullptr);

  Q_SIGNALS:

  private:
    QQmlEngine *display_engine;
  };
} // namespace Launcher
