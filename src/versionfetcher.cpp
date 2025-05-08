#include "versionfetcher.h"

#include <QDebug>

VersionFetcher::VersionFetcher(QString f_user_agent, QNetworkAccessManager *f_net_man, QObject *parent)
    : QObject{parent}
    , agent{f_user_agent}
    , net_man{f_net_man}
{
  qDebug() << "[CTOR]::VERSIONFETCHER:CREATED OBJECT AT" << this;
  qDebug() << "[CTOR]::VERSIONFETCHER:USING QNetworkAccessManager at" << net_man;
}
