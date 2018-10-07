#include "runtimeerror.h"
#include "updater.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>

Updater::Updater(const QString &manifestUrl) : manifestUrl(manifestUrl) {

}

Updater::~Updater() {

}

QString Updater::latestVersion() const {
    if (manifest.isEmpty()) {
        throw RuntimeError("Assertion failed: manifest must be fetched first");
    }

    // manifest.versions[0].version
    return manifest["versions"].toArray()[0].toObject()["version"].toString();
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
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &err);

    if (err.error) {
        throw RuntimeError(tr("There was an error parsing the version file:\n%1")
                           .arg(err.errorString()));
    }

    manifest = document.object();

    delete reply;

    emit fetchManifestComplete();
}

void Updater::install() {
    // The installation process should be transactional, propagating changes
    // to disk only after all steps have completed successfully.
}
