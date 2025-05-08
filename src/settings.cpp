#include "settings.h"
#include "qjsonformat.h"

#include <QDebug>
#include <QSettings>

Launcher::Settings::Settings(QString f_organisation, QString f_application, QObject *parent)
    : QObject{parent}, config{new QSettings("config.json", QJsonFormat::JsonFormat, this)}
{
  qDebug() << "[CTOR]::LAUNCHER::SETTINGS: CREATED OBJECT AT" << this;
}

const QString Launcher::Settings::getClientVersionEndpoint() const
{
  const QString VERSION_ENDPOINT = "https://servers.aceattorneyonline.com/version";
  return config->value("network/version", VERSION_ENDPOINT).toString();
}

void Launcher::Settings::setClientVersionEndpoint()
{
}
