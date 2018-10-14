#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "installnotice.h"
#include "installprogress.h"
#include "options.h"
#include "options.h"
#include "runtimeerror.h"
#include "task.h"
#include "updater.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QNetworkReply>

#include <QSettings>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QtDebug>
#include <QException>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    refresh();
}

MainWindow::~MainWindow() {
    delete ui;
}

QString MainWindow::getVersionFilePath(const QSettings &settings)
{
    // TODO: not DRY
    // Used only in Updater::setCurrentVersion()
    const auto installPath = QDir(settings.value("path", QApplication::applicationDirPath()).toString());
    const auto versionFilePath = installPath.absoluteFilePath("version.ini");
    return versionFilePath;
}

void MainWindow::refresh() {
    const QSettings settings;
    installPath = QDir(Options::getOption<QString>(settings, "path"));
    versionFilePath = installPath.absoluteFilePath("version.ini");

    bool validVersion = false;
    const QSettings versionInfo(versionFilePath, QSettings::Format::IniFormat);
    if (versionInfo.value("program/version").toString() != ""
            && versionInfo.value("assets/version").toString() != "") {
        // XXX: This is a flimsy check.
        validVersion = true;
        ui->labelInstalledVersion->setText(tr("Installed version: %1")
                                           .arg(versionInfo.value("program/version").toString()));
    }

    if (validVersion) {
        ui->stackedWidget->setCurrentWidget(ui->pageReadyToPlay);
        const bool checkUpdates = settings.value("checkOnLaunch", Qt::CheckState::Checked)
                .value<Qt::CheckState>() == Qt::CheckState::Checked;
        if (checkUpdates) {
            checkForUpdates();
        }
    } else {
        ui->stackedWidget->setCurrentWidget(ui->pageNoInstallFound);
    }
}

void MainWindow::play() {
    qDebug() << "Play button clicked";

    const QSettings versionInfo(versionFilePath, QSettings::Format::IniFormat);

    QProcess game(this);
    game.setWorkingDirectory(installPath.absolutePath());
    game.startDetached(versionInfo.value("program/executable").toString());

    if (game.state() != QProcess::NotRunning) {
        this->close();
    }
}

void MainWindow::install() {
    qDebug() << "Install button clicked";
    // ui->stackedWidget->setCurrentWidget(ui->pageInstallProgress);
    if (ui->stackedWidget->currentWidget() == ui->pageNoInstallFound) {
        if (installPath.entryList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries)
                .count() > 0) {
            if (QMessageBox::question(this, tr("Installing in an Occupied Folder"),
                                      tr("You are trying to install in a folder that is not "
                                         "empty! Are you sure you wish to continue?"),
                                      QMessageBox::Ok | QMessageBox::Cancel)
                    != QMessageBox::Ok) {
                return;
            }
        }

        InstallNotice notice(this);
        if (!notice.exec()) {
            return;
        }
    }

    InstallProgress progress(this);
    progress.show();

    const QSettings settings;
    const QSettings versionInfo(versionFilePath, QSettings::Format::IniFormat);

    try {
        for (const QString &package : packages) {
            const auto manifestUrl = Options::getRepositoryUrl(settings, package);
            progress.installProgress(0, tr("Fetching manifest from %1").arg(manifestUrl));

            Updater updater(manifestUrl, package);

            connect(&updater, &Updater::installProgress,
                    &progress, &InstallProgress::installProgress);
            connect(&updater, &Updater::subtaskSetup,
                    &progress, &InstallProgress::subtaskSetup);
            connect(&updater, &Updater::subtaskProgress,
                    &progress, &InstallProgress::subtaskProgress);

            updater.fetchManifest();

            progress.setWindowTitle(tr("Installing %1 (%2)…").arg(package, updater.latestVersion()));
            updater.install(versionInfo.value(package + "/version").toString());
        }
    } catch (const QException &e) {
        QMessageBox::critical(this, tr("Installation Error"), e.what());
    }

    progress.close();

    refresh();
}

void MainWindow::checkForUpdates(bool manual) {
    const QSettings settings;

    try {
        for (const QString &package : packages) {
            Updater updater(Options::getRepositoryUrl(settings, package), package);

            updater.fetchManifest();

            const QSettings versionInfo(versionFilePath, QSettings::Format::IniFormat);
            const QString curVersion = versionInfo.value(package + "/version").toString();

            if (updater.checkForUpdates(curVersion)) {
                const QString newVersion = updater.latestVersion();
                ui->labelUpdateAvailable->setText(tr("Installed %1 version: %2\n"
                                                     "New version: %3\n\n"
                                                     "An update is available.")
                                                  .arg("program", curVersion, newVersion));
                ui->stackedWidget->setCurrentWidget(ui->pageReadyToUpdate);
                return;
            } else {
                ui->stackedWidget->setCurrentWidget(ui->pageReadyToPlay);
            }
        }
    } catch (const QException &e) {
        // Do not print an error message if the update check was automatic
        if (!manual) return;
        QMessageBox::critical(this,
                              tr("Error Checking Updates"),
                              tr("%1\n\n"
                                 "Please ensure that your connection is not blocked by a firewall. "
                                 "If you modified a repository path, please ensure that it is correct.")
                              .arg(e.what()));
    }
}

void MainWindow::showOptions() {
    Options optionsDialog(this);
    if (optionsDialog.exec()) {
        refresh();
    }
}
