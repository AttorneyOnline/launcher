#pragma once

// Never polute the global namespace

#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSettings>

namespace QJsonFormat
{

/*
 * Helpers
 */
QVariant convertToVariant(const QJsonValue &value);
void parseJsonObject(const QJsonObject &object, QStringList key, QSettings::SettingsMap &map);
void parseJsonArray(const QJsonArray &array, QStringList key, QSettings::SettingsMap &map);

QVariant convertToVariant(const QJsonValue &value)
{
  switch (value.type())
  {
  case QJsonValue::Bool:
    return value.toBool();
  case QJsonValue::Double:
    return value.toDouble();
  case QJsonValue::String:
    return value.toString();
  default:
    return QVariant();
  }
}

void parseJsonObject(const QJsonObject &object, QStringList key, QSettings::SettingsMap &map)
{
  const QStringList elements = object.keys();
  for (const QString &element : elements)
  {
    QStringList t_key = key;
    t_key.append(element);
    QJsonValue value = object.value(element);

    if (value.isObject())
    {
      parseJsonObject(value.toObject(), t_key, map);
    }
    else if (value.isArray())
    {
      parseJsonArray(value.toArray(), t_key, map);
    }
    else
    {
      map.insert(t_key.join("/"), convertToVariant(value));
    }
  }
}

void parseJsonArray(const QJsonArray &array, QStringList key, QSettings::SettingsMap &map)
{
  // This is used by Qt to help array reading.
  const int array_size = array.size();

  QStringList size_key = key;
  size_key.append("size");
  map.insert(size_key.join("/"), array_size);

  for (int i = 0; i < array_size; i++)
  {
    QJsonValue element = array.at(i);
    QStringList t_key = key;
    t_key.append(QString::number(i + 1));
    if (element.isObject())
    {
      const QJsonObject obj = element.toObject();
      parseJsonObject(obj, t_key, map);
    }
    else if (element.isArray())
    {
      const QJsonArray arr = element.toArray();
      parseJsonArray(arr, t_key, map);
    }
    else
    {
      map.insert(t_key.join("/"), convertToVariant(element));
    }
  }
}

/*
 * Main Functions
 */

bool readJsonFile(QIODevice &device, QSettings::SettingsMap &map);
bool writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map);

bool readJsonFile(QIODevice &device, QSettings::SettingsMap &map)
{
  QByteArray barr = device.readAll();
  QJsonParseError error;

  QJsonDocument json = QJsonDocument::fromJson(barr, &error);
  if ((error.error != QJsonParseError::NoError) || json.isEmpty() || json.isNull())
  {
    return false;
  }

  if (json.isObject())
  {
    const QJsonObject object = json.object();
    parseJsonObject(object, {}, map);
  }
  else if (json.isArray())
  {
    const QJsonArray array = json.array();
    parseJsonArray(array, {}, map);
  }
  else
  {
    return false;
  }

  return true;
}

bool writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map)
{
  return true;
}

}; // namespace QJsonFormat
