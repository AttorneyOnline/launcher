#pragma once

#include <QObject>
#include <QVersionNumber>

class QNetworkAccessManager;
class QNetworkReply;

class VersionFetcher : public QObject
{
  Q_OBJECT
public:
  explicit VersionFetcher(QString f_user_agent, QNetworkAccessManager *f_net_man = nullptr, QObject *parent = nullptr);

private Q_SLOTS:
  void onReplyReady(QNetworkReply *reply);

private:
  const QString agent;
  QNetworkAccessManager *net_man;

  QVersionNumber version;
};
