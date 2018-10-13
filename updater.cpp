#include "mainwindow.h"
#include "runtimeerror.h"
#include "updater.h"

#include <QLockFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>

Updater::Updater(const QString &manifestUrl, const QString &repoName)
    : manifestUrl(manifestUrl), repoName(repoName) {

}

Updater::~Updater() {

}

QString Updater::latestVersion() const {
    if (manifest.isEmpty()) {
        throw RuntimeError("Assertion failed: manifest must be fetched first");
    }

    // manifest.versions[0].version

    const auto versions = manifest["versions"].toArray();
    if (versions.isEmpty()) {
        throw RuntimeError(tr("There are no versions available for this platform."));
    }

    return versions[0].toObject()["version"].toString();
}

bool Updater::checkForUpdates(const QString &curVersion) const {
    if (manifest.isEmpty()) {
        throw RuntimeError("Assertion failed: manifest must be fetched first");
    }

    // Compare current version to version in JSON
    const QString newVersion = latestVersion();
    return curVersion != newVersion;
}

void Updater::fetchManifest() {
    qDebug() << "Fetching manifest from " << manifestUrl;

    // Craft an HTTP request from path specified in the manifest URL
    QNetworkRequest request(manifestUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    // Perform HTTP GET request
    QNetworkAccessManager http;
    QNetworkReply *reply = http.get(request);

    /*
     * I'm going to take this opportunity to talk about why I do this,
     * despite many people saying "no, don't make local QEventLoops,
     * they are the root of all evil!"
     * From extensive JavaScript experience working with async/await, I
     * detest returning to the days of callback hell where a method's
     * execution is split up into a dozen functions that execute
     * sequentially:
     *
     * doJob()
     * _readConfigFileFinished()
     * _parseConfigFileFinished()
     * _requestFileAFinished()
     * _requestFileBFinished()
     * _writeToConfigFileFinished()
     * doJobFinished()
     *
     * This is extremely irritating and severely restricts variable
     * scope without resorting to turning simple methods into entire
     * worker classes. Therefore, I'd like to research later how to
     * adapt this code into the Tasks library that I imported from
     * here: https://github.com/mhogomchungu/tasks (or make a download
     * worker that facilitates this whole annoying process)
     */
    {
        QEventLoop eventLoop;
        connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        // TODO: add timer
        eventLoop.exec();
    }

    if (reply->error()) {
        throw RuntimeError(tr("There was a network error checking for updates:\n%1")
                           .arg(reply->errorString()));
    }

    // Parse JSON
    QJsonParseError err;
    const QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &err);

    if (err.error) {
        throw RuntimeError(tr("There was an error parsing the version file:\n%1")
                           .arg(err.errorString()));
    }

    manifest = document.object();

    if (manifest["versions"].isUndefined()) {
        throw RuntimeError(tr("The version list was not found inside the version file."));
    }

    // Populate version-to-object map
    for (const QJsonValue versionEntry: manifest["versions"].toArray()) {
        const auto versionObject = versionEntry.toObject();
        versions[versionObject["version"].toString()] = versionObject;
    }

    delete reply;

    emit fetchManifestComplete();
}

void Updater::install(const QString &curVersion) {
    // The installation process should be transactional, propagating changes
    // to disk only after all steps have completed successfully.

    // Hide the subtask progress bar.
    emit subtaskSetup(false);
    emit installProgress(0, tr("Acquiring lock file…"));

    QLockFile lock("launcher.lock");

    // The stale lock time should be at most 15 minutes.
    lock.setStaleLockTime(15 * 60 * 1000);

    // Wait 250 ms for lock to become available if it is locked.
    // Very unlikely that it will unlock during any amount of time if it
    // is currently locked.
    const bool success = lock.tryLock(250);
    if (!success) {
        QString errorMsg;
        switch (lock.error()) {
        case QLockFile::LockFailedError:
            errorMsg = tr("Another instance of the launcher is currently "
                          "performing an install operation.");
            break;
        case QLockFile::PermissionError:
            errorMsg = tr("You do not have permissions to install in this "
                          "folder.");
            break;
        case QLockFile::UnknownError:
        default:
            errorMsg = tr("There was an unknown error writing the lock file "
                          "to disk - is your drive full?");
            break;
        }
        throw RuntimeError(errorMsg);
    }

    // Find out what needs to be downloaded.
    if (latestVersion() == curVersion) {
        throw RuntimeError(tr("Already up-to-date - nothing needs to be done."));
    }

    const auto latestVersion = manifest["versions"].toArray()[0].toObject();
    if (curVersion == nullptr) {
        // Full install
        qDebug() << "No current version specified - assuming full install";
        fullInstall(latestVersion);
    } else {
        // Update
        qDebug() << "Current version specified (" << curVersion << ") - updating";
        update(latestVersion, curVersion);
    }

    emit subtaskSetup(false);
    emit installProgress(100, "All finished!");

    totalTasks = 0;
    completedTasks = 0;

    qDebug() << "All finished!";

    // Lock deleted implicitly (RAII)
}

void Updater::fullInstall(const QJsonObject &version) {
    const auto fullInstallTasks = version["full"].toArray();
    const QString prev = version["prev"].toString();
    const QString current = version["version"].toString();

    qDebug() << "Performing full install: " << current;

    totalTasks += fullInstallTasks.count();

    if (fullInstallTasks.isEmpty()) {
        // This version is update-only, so we have to look for the previous
        // version and check if that one has tasks for a full download.
        // If not, then we have to do it recursively...
        const auto previousVersion = versions[prev];

        if (previousVersion.isEmpty()) {
            throw RuntimeError(tr("Failed to install %1 - dependency tree is broken!")
                               .arg(current));
        }

        fullInstall(previousVersion);
        update(version, prev);
    } else {
        // Perform full install tasks
        emit installProgress(getInstallProgress(), tr("Installing %1…").arg(current));

        for (const auto &task : fullInstallTasks) {
            performTask(task.toObject());
        }

        setCurrentVersion(version);
    }

}

void Updater::update(const QJsonObject &version, const QString &fromVersion) {
    const auto updateTasks = version["update"].toArray();
    const auto fullInstallTasks = version["full"].toArray();
    const QString current = version["version"].toString();
    const QString prev = version["prev"].toString();

    qDebug() << "Performing update: " << current;

    totalTasks += updateTasks.count();

    if (updateTasks.isEmpty() && !fullInstallTasks.isEmpty()) {
        // This version is full install only, so we cannot update.
        // Attempt a clean install.
        return fullInstall(version);
    }

    if (fromVersion != prev) {
        // Walk down the dependency tree of previous versions
        update(versions[prev], fromVersion);
    }

    qDebug() << prev << " -> " << current;

    // Perform update tasks
    emit installProgress(getInstallProgress(), tr("Updating from %1 to %2…")
                         .arg(prev, current));

    for (const auto &task : updateTasks) {
        performTask(task.toObject());
    }

    setCurrentVersion(version);
}

void Updater::performTask(const QJsonObject &task) {
    const QString action = task["action"].toString();
    if (action == "dl") {
        const QString url = task["url"].toString();
        taskDownload(url);
    } else if (action == "delete") {
        const QString target = task["target"].toString();
        taskDelete(target);
    } else if (action == "deleteDir") {
        const QString target = task["target"].toString();
        taskDeleteDir(target);
    } else {
        qWarning() << "Unknown task: " << action;
    }

    completedTasks++;
    emit installProgress(getInstallProgress());
}

void Updater::taskDownload(const QString &url) {
    emit subtaskSetup(true);
    emit subtaskProgress(0, tr("Downloading %1").arg(url));

    // TODO: download and unzip file to installation directory
    // Check SHA-1 checksum of file
}

void Updater::taskDelete(const QString &target) {
    emit subtaskSetup(true);
    emit subtaskProgress(100, tr("Deleting file %1").arg(target));

    // TODO: delete single file WITHIN the installation directory!
}

void Updater::taskDeleteDir(const QString &target) {
    emit subtaskSetup(true);
    emit subtaskProgress(100, tr("Deleting directory %1").arg(target));

    // TODO: delete directory and all of its contents recursively
    // WITHIN the installation directory!
}

void Updater::setCurrentVersion(const QJsonObject &version) {
    qDebug() << "Setting current version to " << version;

    const QString current = version["version"].toString();
    const QString executablePath = version["executable"].toString();

    const QSettings settings;
    QSettings currentVersionInfo(MainWindow::getVersionFilePath(settings));
    currentVersionInfo.setValue(repoName + "/version", current);

    if (executablePath != "") {
        currentVersionInfo.setValue(repoName + "/executable", executablePath);
    }
}
