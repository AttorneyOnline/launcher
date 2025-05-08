#include "manifestfetcher.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

ManifestFetcher::ManifestFetcher(QString f_user_agent, QNetworkAccessManager *f_net_man, QObject *parent)
    : QObject{parent}
    , agent{f_user_agent}
    , net_man{f_net_man}
{
  qDebug() << "[CTOR]::MANIFESTFETCHER:CREATED OBJECT AT" << this;
  qDebug() << "[CTOR]::MANIFESTFETCHER:USING QNetworkAccessManager at" << net_man;
}

void ManifestFetcher::fetch(QString url)
{
  QNetworkRequest req;
  req.setHeader(QNetworkRequest::UserAgentHeader, agent);
  req.setUrl(QUrl(url));

  QNetworkReply *reply = net_man->get(req);
  connect(reply, &QNetworkReply::finished, this, std::bind(&ManifestFetcher::onReplyReady, this, reply));
}

void ManifestFetcher::onReplyReady(QNetworkReply *reply)
{
  QScopedPointer<QNetworkReply> data(reply);
  QNetworkReply::NetworkError error = data->error();

  if (error != QNetworkReply::NoError)
  {}
}
