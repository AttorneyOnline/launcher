#pragma once

#include "settings.h"

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

namespace Launcher
{
  class WebAccess : public QObject
  {
    Q_OBJECT
  public:
    WebAccess(QString f_user_agent, Settings *f_settings, QNetworkAccessManager *f_net_man = nullptr, QObject *parent = nullptr);

    QNetworkReply *reply(Endpoint endpoint);

    void fetch(Endpoint document);

  private Q_SLOTS:
    void onReplyReady(Endpoint document, QNetworkReply *reply);

  private:
    Settings *settings;
    QNetworkAccessManager *webaccess;
    const QString user_agent;
  };
} // namespace Launcher
