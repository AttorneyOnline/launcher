#pragma once

#include <QObject>
#include <QVersionNumber>

class QNetworkAccessManager;

namespace Launcher
{
  class Settings;
  class WebAccess;
  class LauncherUI;

  class LauncherCore : public QObject
  {
    Q_OBJECT
  public:
    LauncherCore(QNetworkAccessManager *f_net_man = nullptr, Settings *f_settings = nullptr, QObject *parent = nullptr);
    ~LauncherCore() = default;

  private:
    WebAccess *webaccess;
    Settings *settings;
    LauncherUI *user_interface;

    QVersionNumber client_target_version;
    QVersionNumber launcher_target_version;
  };
}; // namespace Launcher
