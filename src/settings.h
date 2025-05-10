#pragma once

#include "datatypes.h"
#include <QObject>

class QSettings;

namespace Launcher
{
  class Settings : public QObject
  {
    Q_OBJECT
  public:
    explicit Settings(QString f_organisation, QString f_application, QObject *parent = nullptr);

    const QString getEndpoint(Endpoint endpoint) const;
    void setEndpoint(Endpoint endpoint, const QString &url);

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
