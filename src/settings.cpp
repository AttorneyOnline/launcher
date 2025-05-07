#include "settings.h"
#include "qjsonformat.h"

#include <QDebug>
#include <QSettings>

Launcher::Settings::Settings(QString f_organisation, QString f_application, QObject *parent)
    : QObject{parent}
{
  const QSettings::Format JsonFormat = QSettings::registerFormat("json", &QJsonFormat::readJsonFile, &QJsonFormat::writeJsonFile);

  QSettings userConfig("config.json", JsonFormat, this);
  qDebug() << userConfig.allKeys();
  qDebug() << userConfig.value("foo/1");
  qDebug() << userConfig.value("foo/2");

  userConfig.sync();
}
