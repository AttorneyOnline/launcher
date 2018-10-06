#include "options.h"
#include "ui_options.h"

#include <QFileDialog>
#include <QSettings>
#include <QtDebug>

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options) {
    ui->setupUi(this);

    QSettings settings;
    ui->tbInstallPath->setText(settings.value("path").value<QString>());
    ui->tbProgramRepo->setText(settings.value("repos/program").value<QString>());
    ui->tbAssetRepo->setText(settings.value("repos/assets").value<QString>());
    ui->cbCheckForUpdatesOnLaunch->setCheckState(settings.value("checkOnLaunch", Qt::CheckState::Checked).value<Qt::CheckState>());
}

Options::~Options() {
    delete ui;
}

void Options::on_btnBrowseInstallPath_clicked() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Browse Install Path"));
    if (path != "") {
        ui->tbInstallPath->setText(path);
    }
}

void Options::accept() {
    QSettings settings;
    settings.setValue("path", ui->tbInstallPath->text());
    settings.setValue("repos/program", ui->tbProgramRepo->text());
    settings.setValue("repos/assets", ui->tbAssetRepo->text());
    settings.setValue("checkOnLaunch", ui->cbCheckForUpdatesOnLaunch->checkState());
    qDebug() << "Wrote settings to file";
    done(QDialog::Accepted);
}
