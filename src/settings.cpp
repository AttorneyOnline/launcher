#include "settings.h"
#include "qjsonformat.h"

#include <QDebug>
#include <QSettings>
#include <QtAssert>

Launcher::Settings::Settings(QString f_organisation, QString f_application, QObject *parent)
    : QObject{parent}
    , config{new QSettings("config.json", QJsonFormat::JsonFormat, this)}
{
  qDebug() << "[CTOR]::LAUNCHER::SETTINGS: CREATED OBJECT AT" << this;
}

const QString Launcher::Settings::getEndpoint(Endpoint endpoint) const
{
  switch (endpoint)
  {
  case Endpoint::CLIENTVERSION:
    return config->value("endpoints/client", "https://servers.aceattorneyonline.com/version").toString();
    break;
  case Endpoint::LAUNCHERVERSION:
    return config->value("endpoints/launcher", "https://servers.aceattorneyonline.com/launcher").toString();
    break;
  case Endpoint::MEDIAREPOSITORY:
    return config->value("endpoints/media", "https://servers.aceattorneyonline.com/media").toString();
    break;
  default:
    Q_UNREACHABLE();
    break;
  }
}

void Launcher::Settings::setEndpoint(Endpoint endpoint, const QString &url)
{
  switch (endpoint)
  {
  case Endpoint::CLIENTVERSION:
    return config->setValue("endpoints/client", url);
    break;
  case Endpoint::LAUNCHERVERSION:
    config->setValue("endpoints/launcher", url);
    break;
  case Endpoint::MEDIAREPOSITORY:
    config->setValue("endpoints/media", url);
    break;
  }
}
