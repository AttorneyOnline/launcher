#ifndef UPDATER_H
#define UPDATER_H

#include <QJsonObject>
#include <QObject>
#include <QString>

class Updater : public QObject
{
    Q_OBJECT

public:
    Updater(const QString &manifestUrl);
    ~Updater();

    bool checkForUpdates(const QString &oldVersion);
    void fetchManifest();

signals:
    void fetchManifestComplete();

private:
    QString manifestUrl;
    QJsonObject manifest;
};

#endif // UPDATER_H
