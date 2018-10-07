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

    QString latestVersion() const;
    bool checkForUpdates(const QString &oldVersion) const;
    void fetchManifest();
    void install();

signals:
    void fetchManifestComplete();

    void installProgress(int progress, const QString &msg = nullptr);
    void subtaskSetup(bool enabled, int max = 100);
    void subtaskProgress(int progress, const QString &msg = nullptr);

private:
    QString manifestUrl;
    QJsonObject manifest;
};

#endif // UPDATER_H
