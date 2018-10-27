#ifndef UPDATER_H
#define UPDATER_H

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QString>

class Updater : public QObject
{
    Q_OBJECT

public:
    Updater(const QString &manifestUrl, const QString &repoName);
    ~Updater();

    QString latestVersion() const;
    bool checkForUpdates(const QString &curVersion) const;
    void fetchManifest();
    void install(const QString &curVersion = nullptr);

signals:
    void fetchManifestComplete();

    void installProgress(int progress, const QString &msg = nullptr);
    void subtaskSetup(bool enabled, int max = 100);
    void subtaskProgress(int progress, const QString &msg = nullptr);

private:
    const QString manifestUrl;
    const QString repoName;

    QJsonObject manifest;
    std::map<QString, QJsonObject> versions;

    int totalTasks = 0;
    int completedTasks = 0;

    int getInstallProgress() const {
        return static_cast<int>(completedTasks / static_cast<double>(totalTasks) * 100);
    }

    void fullInstall(const QJsonObject &version);
    void update(const QJsonObject &version, const QString &fromVersion);

    void performTask(const QJsonObject &task);

    void taskDownload(QDir &installDir, const QUrl &url, const QString &hash);
    void taskDelete(QDir &installDir, const QString &target);
    void taskDeleteDir(QDir &installDir, const QString &target);
    void taskNotice(const QString &msg);

    void setCurrentVersion(const QJsonObject &version);
};

#endif // UPDATER_H
