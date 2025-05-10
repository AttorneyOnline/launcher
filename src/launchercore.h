#pragma once

#include <QObject>
#include <QVersionNumber>

class QNetworkAccessManager;

namespace Launcher
{
  class Settings;
  class WebAccess;
} // namespace Launcher

namespace Launcher
{
  class LauncherCore : public QObject
  {
    Q_OBJECT
  public:
    LauncherCore(QNetworkAccessManager *f_net_man = nullptr, Launcher::Settings *f_settings = nullptr, QObject *parent = nullptr);
    ~LauncherCore() = default;

  private:
    WebAccess *webaccess;
    Settings *settings;

    QVersionNumber client_target_version;
    QVersionNumber launcher_target_version;
  };
}; // namespace Launcher
