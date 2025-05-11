#include "webaccess.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

using namespace Launcher;

WebAccess::WebAccess(QString f_user_agent, Settings *f_settings, QNetworkAccessManager *f_net_man, QObject *parent)
    : QObject{parent}
    , settings{f_settings}
    , webaccess{f_net_man}
    , user_agent{f_user_agent}
{
  qDebug() << "[CTOR]::WEBACCESS:CREATED OBJECT AT" << this;
  qDebug() << "[CTOR]::MANIFESTFETCHER:USING QNetworkAccessManager at" << webaccess;
}

void WebAccess::fetch(Endpoint document)
{
  QNetworkRequest req;
  req.setHeader(QNetworkRequest::UserAgentHeader, user_agent);
  req.setUrl(settings->getEndpoint(document));

  QNetworkReply *reply = webaccess->get(req);
  connect(reply, &QNetworkReply::finished, this, std::bind(&WebAccess::onReplyReady, this, document, reply));
}

void WebAccess::onReplyReady(Endpoint document, QNetworkReply *reply)
{
  QScopedPointer<QNetworkReply> data(reply);
  QNetworkReply::NetworkError error = data->error();

  if (error != QNetworkReply::NoError)
  {
    qDebug() << "WEBACCESS::ONREPLYREADY: Failed to fetch document at endpoint" << document;
    qDebug() << "WEBACCESS::ONREPLYREADY:" << data->errorString();

    return;
  }

  qDebug() << "WEBACCESS::ONREPLYREADY: Received valid response from endpoint" << document;
  qDebug() << "WEBACCESS::ONREPLYREADY:" << data->readAll();
}
