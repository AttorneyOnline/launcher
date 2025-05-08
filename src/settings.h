#pragma once

#include <QObject>

class QSettings;

namespace Launcher
{
class Settings : public QObject
{
  Q_OBJECT
public:
  explicit Settings(QString f_organisation, QString f_application, QObject *parent = nullptr);

  // Network Settings
  const QString getLauncherVersionEndpoint() const;
  void setLauncherVersionEndpoint(const QString &endpoint);

  const QString getClientVersionEndpoint() const;
  void setClientVersionEndpoint(const QString &endpoint);

  const QString getMediaEndpoint() const;
  void setMediaEndpoint(const QString &endpoint);

  // User Settings
  QString applicationInstallPath();
  void setApplicationInstallPath(const QString &directory);

  QString mediaInstallationPath();
  void setMediaInstallationPath(const QString &directory);

  bool checkForUpdateOnLaunch() const;
  void setCheckForUpdateOnLaunch(bool state);

  void reset();

private:
  QSettings *config;
};
} // namespace Launcher
