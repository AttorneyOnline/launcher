#pragma once

#include <QObject>

namespace Launcher
{
class Settings : public QObject
{
  Q_OBJECT
public:
  explicit Settings(QString f_organisation, QString f_application, QObject *parent = nullptr);

  QString applicationInstallPath();
  void setApplicationInstallPath();

  QString mediaInstallationPath();
  void setMediaInstallationPath();

  bool checkForUpdateOnLaunch() const;
  void setCheckForUpdateOnLaunch();

  void reset();
};
} // namespace Launcher
