#include "mainwindow.h"
#include "runtimeerror.h"
#include "options.h"
#include "updater.h"
#include "aria2client.h"

#include <QArchive>

#include <QLockFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QDir>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSaveFile>
#include <QSettings>
#include <memory>
#include <utility>

extern template QString Options::getOption<QString>(const QSettings &settings, const QString &option);

Updater::Updater(QString manifestUrl, QString repoName)
    : manifestUrl(std::move(manifestUrl))
    , repoName(std::move(repoName)) {

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
    qDebug() << "Fetching manifest from" << manifestUrl;

    // Craft an HTTP request from path specified in the manifest URL
    QNetworkRequest request(manifestUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);

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
        connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        // TODO: add timer
        eventLoop.exec();
    }

    if (reply->error()) {
        throw RuntimeError(tr("There was a network error checking for updates:\n%1")
                           .arg(reply->errorString()));
    }

    // Parse JSON
    QJsonParseError err{};
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

bool Updater::hasBranding() {
    return !manifest["launcher"].toObject()["branding"].toString().isEmpty();
}

QByteArray Updater::fetchBranding() {
    QString brandingUrl = manifest["launcher"].toObject()["branding"].toString();
    qDebug() << "Fetching branding from" << brandingUrl;

    // Craft an HTTP request from path specified in the manifest URL
    QNetworkRequest request(brandingUrl);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);

    // Perform HTTP GET request
    QNetworkAccessManager http;
    QNetworkReply *reply = http.get(request);
    {
        QEventLoop eventLoop;
        connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        // TODO: add timer
        eventLoop.exec();
    }

    if (reply->error()) {
        qWarning() << "Network error getting branding:"
                   << reply->errorString();
        return {};
    }

    // Get resource file data
    QByteArray data = reply->readAll();
    delete reply;

    return data;
}

void Updater::cancel() {
    if (eventLoop != nullptr) {
        eventLoop->exit(-1);
    }
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
        qDebug() << "Already up-to-date - nothing needs to be done";
        return;
    }

    const auto latestVersion = manifest["versions"].toArray()[0].toObject();
    if (curVersion == nullptr || curVersion.isEmpty()) {
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

    qDebug() << "Performing full install:" << current;

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

    while (!downloadedFiles.empty()) {
        QString file = downloadedFiles.front();
        qDebug() << "deleting temp file" << file;
        QFile::remove(file);
        downloadedFiles.pop_front();
    }
}

void Updater::update(const QJsonObject &version, const QString &fromVersion) {
    const auto updateTasks = version["update"].toArray();
    const auto fullInstallTasks = version["full"].toArray();
    const QString current = version["version"].toString();
    const QString prev = version["prev"].toString();

    qDebug() << "Performing update:" << current;

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

    qDebug() << prev << "->" << current;

    // Perform update tasks
    emit installProgress(getInstallProgress(), tr("Updating from %1 to %2…")
                         .arg(prev, current));

    for (const auto &task : updateTasks) {
        performTask(task.toObject());
    }

    while (!downloadedFiles.empty()) {
        QString file = downloadedFiles.front();
        qDebug() << "deleting temp file" << file;
        QFile::remove(file);
        downloadedFiles.pop_front();
    }

    setCurrentVersion(version);
}

void Updater::performTask(const QJsonObject &task) {
    const QSettings settings;
    QDir dir(Options::getOption<QString>(settings, "path"));

    const QString action = task["action"].toString();
    if (action == "dl") {
        const QString url = task["url"].toString();
        const QString hash = task["hash"].toString();
        taskDownload(dir, QUrl(url), hash);
    } else if (action == "delete") {
        if (task["target"].isArray()) {
            QStringList targets;
            for (const QJsonValue val : task["target"].toArray())
                targets.append(val.toString());

            taskDelete(dir, targets);
        } else {
            taskDelete(dir, {task["target"].toString()});
        }
    } else if (action == "deleteDir") {
        if (task["target"].isArray()) {
            QStringList targets;
            for (const QJsonValue val : task["target"].toArray())
                targets.append(val.toString());

            taskDeleteDir(dir, targets);
        } else {
            taskDeleteDir(dir, {task["target"].toString()});
        }
    } else if (action == "notice") {
        const QString msg = task["msg"].toString();
        const bool versionCheck = task["versionCheck"].toBool();
        if (versionCheck) {
            const QString launcherVersion = Options::getOption<QString>(settings, "version");
            const QString manifestLauncherVersion = manifest["launcher"].toObject()
                    ["version"].toString();
            qDebug() << "Launcher version:" << launcherVersion;
            qDebug() << "Manifest's launcher version:" << manifestLauncherVersion;
            if (versionCheck && !manifestLauncherVersion.isEmpty()
                    && launcherVersion != manifestLauncherVersion) {
                taskNotice(msg);
            }
        } else {
            taskNotice(msg);
        }
    } else if (action == "move") {
        const QString source = task["source"].toString();
        const QString target = task["target"].toString();
        taskMove(dir, source, target);
    } else {
        qWarning() << "Unknown task:" << action;
    }

    completedTasks++;
    emit installProgress(getInstallProgress());
}

void Updater::taskDownload(QDir &installDir, const QUrl &url, const QString &hash) {
    emit subtaskSetup(true);
    emit subtaskProgress(0, tr("Downloading %1").arg(url.toDisplayString()));

    qDebug() << "task: dl" << url;

    // Create download directory
    installDir.mkpath("download");
    installDir.cd("download");

    // Create file within the download directory
    const QString filename = installDir.filePath(url.fileName());
    QFile file(filename);

    // Skip download if hash is correct
    bool skipDownload = false;
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly)) {
            throw RuntimeError(tr("Could not read download file: %1")
                               .arg(file.errorString()));
        }

        if (!hash.isEmpty()) {
            emit subtaskProgress(0, tr("Calculating checksum of existing file %1")
                .arg(url.toDisplayString()));
            QCryptographicHash sha1(QCryptographicHash::Sha1);
            sha1.addData(&file);
            if (sha1.result().toHex() == hash) {
                skipDownload = true;
            } else {
                qWarning() << "checksum mismatch: expected" << hash
                           << "but got" << sha1.result().toHex();
            }
        }

        file.close();
    }

    if (!skipDownload) {
        // Download file
        eventLoop = std::make_unique<QEventLoop>();

        std::unique_ptr<Aria2Client> downloader = std::make_unique<Aria2Client>(this);
        connect(downloader.get(), &Aria2Client::progress, [this, url](uint64_t current, uint64_t max, uint64_t speed) {
            int downloadPercent = (int) ((double) current / (double) max * 100.0);
            int subPercent = (int) ((double) (downloadPercent / 200.0) / totalTasks * 100.0);
            emit installProgress(getInstallProgress() + subPercent);
            emit subtaskProgress(downloadPercent, tr("[%1 KB/s] %2 (%3 MB)")
                                 .arg(QLocale::system().toString((double) speed / 1024, 'f', 1),
                                      url.toString(),
                                      QLocale::system().toString((double) max / (1024 * 1024), 'f', 1)));
        });
        connect(downloader.get(), &Aria2Client::finished, eventLoop.get(), &QEventLoop::exit);

        downloader->download(url.toString(), hash, filename);

        int returnCode = eventLoop->exec();
        eventLoop = nullptr;
        if (returnCode == -1) {
            throw RuntimeError(tr("The installation was canceled by the user."));
        } else if (returnCode) {
            throw RuntimeError(Aria2Client::errorString(static_cast<DownloadError>(returnCode)));
        }
    }

    emit subtaskProgress(100, tr("Extracting %1").arg(file.fileName()));

    // TODO: unzip files to temp directory first and then move them to install directory
    installDir.cdUp();

    bool error = false;
    QString errorMsg;
    short errorCode;

    {
        // (Yeah, we're gonna do this crappy trick again...)
        eventLoop = std::make_unique<QEventLoop>();
        auto *extractor = new QArchive::DiskExtractor(this, false);
        extractor->setArchive(filename, installDir.path());
        extractor->setCalculateProgress(true);
        QObject::connect(extractor, &QArchive::Extractor::error,
                         [&](short code) {
            error = true;
            errorMsg = QArchive::errorCodeToString(code);
            errorCode = code;
            qCritical() << "Error extracting archive:" << errorMsg << "- code" << errorCode;
            emit eventLoop->quit();
        });
        QObject::connect(extractor, &QArchive::Extractor::progress,
                         [this](const QString &file, int filesDone, int totalFiles,
                                    qint64 bytesProcessed, qint64 bytesTotal) {
            int extractPercent = (int) ((double) bytesProcessed / (double) bytesTotal * 100.0);
            int subPercent = (int) ((double) (extractPercent / 200.0 + 0.5) / totalTasks * 100.0);
            emit installProgress(getInstallProgress() + subPercent);
            emit subtaskProgress(extractPercent, tr("[%1/%2] %3")
                                 .arg(filesDone)
                                 .arg(totalFiles)
                                 .arg(file));
        });
        QObject::connect(extractor, &QArchive::Extractor::diskFinished,
                         eventLoop.get(), &QEventLoop::quit);

        extractor->setBlockSize(16384); // 16K blocks
        extractor->start();

        int returnCode = eventLoop->exec();
        eventLoop = nullptr;
        if (returnCode) {
            extractor->cancel();
            throw RuntimeError(tr("The installation was canceled by the user."));
        }
    }

    if (error) {
        throw RuntimeError(tr("Unable to extract from archive: %1 (error code %2)")
                           .arg(errorMsg)
                           .arg(errorCode));
    }

    // If the extraction was successful, delete the archive later
    downloadedFiles.push_back(filename);
}

void Updater::taskDelete(QDir &installDir, const QStringList &targets) {
    emit subtaskSetup(true);

    int completed = 0, total = targets.length();
    for (const QString &target : targets) {
        emit subtaskProgress(static_cast<int>(static_cast<double>(completed) / total),
                             tr("[%1/%2] Deleting file %3")
                             .arg(completed).arg(total)
                             .arg(target));

        qDebug() << "task: delete" << target;

        // Delete single file only if it is within the installation directory
        QFileInfo fileInfo(installDir.filePath(target));

        if (!fileInfo.canonicalFilePath().startsWith(installDir.canonicalPath())) {
            qWarning() << target << ": ignoring invalid path!";
        } else {
            QFile::remove(installDir.filePath(target));
        }

        completed++;
    }
}

void Updater::taskDeleteDir(QDir &installDir, const QStringList &targets) {
    emit subtaskSetup(true);

    int completed = 0, total = targets.length();
    for (const QString &target : targets) {
        emit subtaskProgress(static_cast<int>(static_cast<double>(completed) / total),
                             tr("[%1/%2] Deleting directory %3")
                             .arg(completed).arg(total)
                             .arg(target));

        qDebug() << "task: deleteDir" << target;

        // Delete directory and all of its contents recursively
        // only if it is within the installation directory
        QDir dir(installDir.filePath(target));

        if (!dir.canonicalPath().startsWith(installDir.canonicalPath())
                || dir.canonicalPath() == installDir.canonicalPath()) {
            qWarning() << target << ": ignoring invalid path!";
        } else {
            dir.removeRecursively();
        }

        completed++;
    }
}

void Updater::taskNotice(const QString &msg)
{
    emit subtaskSetup(false);

    if (QMessageBox::information(nullptr, "Install Notice", msg,
                                 QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel) {
        throw RuntimeError(tr("The installation was canceled by the user."));
    }
}

void Updater::taskMove(QDir &installDir, const QString &source, const QString &target)
{
    emit subtaskSetup(false);
    emit subtaskProgress(100, tr("Moving %1 to %2").arg(source, target));

    // Move only if source and target are within the installation directory
    if (!installDir.absoluteFilePath(source).startsWith(installDir.absolutePath())
            || !installDir.absoluteFilePath(target).startsWith(installDir.absolutePath())) {
        qWarning() << target << ": ignoring invalid path!";
    } else {
        installDir.rename(source, target);
    }
}

void Updater::setCurrentVersion(const QJsonObject &version) {
    const QString current = version["version"].toString();
    const QString executablePath = version["executable"].toString();

    qDebug() << "Setting current version to" << current;

    const QSettings settings;
    QSettings currentVersionInfo(MainWindow::getVersionFilePath(settings),
                                 QSettings::IniFormat);
    currentVersionInfo.setValue(repoName + "/version", current);

    if (executablePath != "") {
        currentVersionInfo.setValue(repoName + "/executable", executablePath);
    }
}
