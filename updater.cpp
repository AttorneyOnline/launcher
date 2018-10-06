#include "runtimeerror.h"
#include "updater.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>

Updater::Updater(const QString &manifestUrl) : manifestUrl(manifestUrl)
{

}

bool Updater::checkForUpdates(const QString &curVersion) {
    // Craft an HTTP request from path specified in `repos/program`
    const QSettings settings;
    QNetworkRequest request(settings.value("repos/program").value<QString>());
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());

    try {
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
            connect(reply, SIGNAL(finished()), &eventLoop, SLOT(exit()));
            // TODO: add timer
            eventLoop.exec();
        }

        if (reply->error()) {
            throw RuntimeError(tr("There was a network error checking for updates: %1")
                               .arg(reply->errorString()));
        }

        // Parse JSON
        QJsonParseError err;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &err);
        QJsonObject json = document.object();

        if (err.error) {
            throw RuntimeError(tr("There was an error parsing the version file: %1")
                               .arg(err.errorString()));
        }

        delete reply;

        // Compare current version to version in JSON

        const QString newVersion = json["versions"].toArray()[0].toObject()["version"].toString();
        if (curVersion != newVersion) {
            ui->labelUpdateAvailable->setText(tr("Installed %1 version: %2\n"
                                                 "New version: %3\n\n"
                                                 "An update is available.")
                                              .arg("program", curVersion, newVersion));
            ui->stackedWidget->setCurrentWidget(ui->pageReadyToUpdate);
        } else {
            ui->stackedWidget->setCurrentWidget(ui->pageReadyToPlay);
        }
    } catch (const QException &e) {
        if (manual) {
            QMessageBox::critical(this,
                                  tr("Error Checking Updates"),
                                  tr("%1\n"
                                     "Please ensure that your connection is not blocked by a firewall. "
                                     "If you modified a repository path, please ensure that it is correct.")
                                  .arg(e.what()));
        }
    }
}
