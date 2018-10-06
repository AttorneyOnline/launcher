#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "options.h"
#include "runtimeerror.h"
#include "task.h"

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

void MainWindow::refresh() {
    const QSettings settings;
    installPath = QDir(settings.value("path", QApplication::applicationDirPath()).value<QString>());
    versionFilePath = installPath.absoluteFilePath("version.ini");

    bool validVersion = false;
    const QSettings versionInfo(versionFilePath, QSettings::Format::IniFormat);
    if (versionInfo.value("program/version").value<QString>() != ""
            && versionInfo.value("assets/version").value<QString>() != "") {
        validVersion = true;
    }

    if (validVersion) {
        ui->stackedWidget->setCurrentWidget(ui->pageReadyToPlay);
        const bool checkUpdates = settings.value("checkOnLaunch", Qt::CheckState::Checked).value<Qt::CheckState>() == Qt::CheckState::Checked;
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
    ui->stackedWidget->setCurrentWidget(ui->pageInstallProgress);
}

void MainWindow::checkForUpdates(bool manual) {
    const QSettings versionInfo(versionFilePath, QSettings::Format::IniFormat);
    const QString curVersion = versionInfo.value("program/version").toString();
}

void MainWindow::showOptions() {
    Options optionsDialog(this);
    if (optionsDialog.exec()) {
        refresh();
    }
}
