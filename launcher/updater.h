#ifndef UPDATER_H
#define UPDATER_H

#include <QDir>
#include <QEventLoop>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include <memory>

class Updater : public QObject
{
    Q_OBJECT

public:
    Updater(QString manifestUrl, QString repoName);

    QString latestVersion() const;
    bool checkForUpdates(const QString &curVersion) const;
    void fetchManifest();
    void install(const QString &curVersion = nullptr);

    bool hasBranding();
    QByteArray fetchBranding();
signals:
    void fetchManifestComplete();

    void installProgress(int progress, const QString &msg = nullptr);
    void subtaskSetup(bool enabled, int max = 100);
    void subtaskProgress(int progress, const QString &msg = nullptr);

public slots:
    void cancel();

private:
    const QString manifestUrl;
    const QString repoName;

    QJsonObject manifest;
    std::map<QString, QJsonObject> versions;

    int totalTasks = 0;
    int completedTasks = 0;

    // For long-running tasks only.
    std::unique_ptr<QEventLoop> eventLoop;

    // Downloaded files to be removed at the end of a transaction
    std::list<QString> downloadedFiles;

    int getInstallProgress() const {
        return static_cast<int>(completedTasks / static_cast<double>(totalTasks) * 100);
    }

    void fullInstall(const QJsonObject &version);
    void update(const QJsonObject &version, const QString &fromVersion);

    void performTask(const QJsonObject &task);

    void taskDownload(QDir &installDir, const QUrl &url, const QString &hash);
    void taskDelete(QDir &installDir, const QStringList &targets);
    void taskDeleteDir(QDir &installDir, const QStringList &targets);
    void taskNotice(const QString &msg);
    void taskMove(QDir &installDir, const QString &source, const QString &target);

    void setCurrentVersion(const QJsonObject &version);
};

#endif // UPDATER_H
