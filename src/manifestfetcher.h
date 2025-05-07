#pragma once

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class ManifestFetcher : public QObject
{
  Q_OBJECT
public:
  ManifestFetcher(QString f_user_agent, QNetworkAccessManager *f_net_man = nullptr, QObject *parent = nullptr);

  void fetch(QString url);

Q_SIGNALS:


private Q_SLOTS:
  void onReplyReady(QNetworkReply *reply);

private:
  QString agent;
  QNetworkAccessManager *net_man;
};
