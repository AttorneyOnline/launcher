#pragma once

#include "settings.h"

#include <QObject>

class ManifestFetcher;
class QNetworkAccessManager;

class LauncherCore : public QObject
{
  Q_OBJECT
public:
  LauncherCore(Launcher::Settings* f_settings = nullptr, QObject *parent = nullptr);
  ~LauncherCore() = default;

  QNetworkAccessManager *net_man;
  ManifestFetcher *fetcher;
  Launcher::Settings *settings;
};
