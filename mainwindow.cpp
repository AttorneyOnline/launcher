#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "installnotice.h"
#include "installprogress.h"
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
    installPath = QDir(settings.value("path", QApplication::applicationDirPath()).toString());
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
}

void MainWindow::install() {
    qDebug() << "Install button clicked";
    // ui->stackedWidget->setCurrentWidget(ui->pageInstallProgress);
    if (ui->stackedWidget->currentWidget() == ui->pageNoInstallFound) {
        InstallNotice notice(this);
        if (!notice.exec()) {
            return;
        }
    }

    InstallProgress progress(this);
    progress.show();

    // TODO: iterate through packages that need installation/update
}

void MainWindow::checkForUpdates(bool manual) {
    const QSettings settings;

    try {
        for (const QString &package : packages) {
            Updater updater(settings.value("repos/" + package).toString(), package);

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
