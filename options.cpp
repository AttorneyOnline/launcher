#include "options.h"
#include "ui_options.h"

#include <QFileDialog>
#include <QSettings>
#include <QtDebug>

const std::map<const QString, const QVariant> Options::defaultOptions = {
    {"path", QApplication::applicationDirPath()},
    {"repos/program", "https://assets.aceattorneyonline.com/program_${os}.json"},
    {"repos/assets", "https://assets.aceattorneyonline.com/assets.json"},
    {"checkOnLaunch", Qt::CheckState::Checked},
};

template <class T>
T Options::getOption(QSettings &settings, const QString &option) {
    return settings.value(option, defaultOptions.at(option)).value<T>();
}

Options::Options(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Options) {
    ui->setupUi(this);

    QSettings settings;
    ui->tbInstallPath->setText(settings.value("path").toString());
    // ui->tbInstallPath->setPlaceholderText(defaultOptions.at("path").toString());

    ui->tbProgramRepo->setText(settings.value("repos/program").toString());
    ui->tbProgramRepo->setPlaceholderText(defaultOptions.at("repos/program").toString());

    ui->tbAssetRepo->setText(settings.value("repos/assets").toString());
    ui->tbAssetRepo->setPlaceholderText(defaultOptions.at("repos/assets").toString());

    ui->cbCheckForUpdatesOnLaunch->setCheckState(settings.value("checkOnLaunch", Qt::CheckState::Checked)
                                                 .value<Qt::CheckState>());
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
    const std::map<QString, QVariant> options = {
        {"path", ui->tbInstallPath->text()},
        {"repos/program", ui->tbProgramRepo->text()},
        {"repos/assets", ui->tbAssetRepo->text()},
        {"checkOnLaunch", ui->cbCheckForUpdatesOnLaunch->checkState()}
    };
    for (const auto &option : options) {
        if (option.second != "") {
            settings.setValue(option.first, option.second);
        }
    }
    qDebug() << "Wrote settings to file";
    done(QDialog::Accepted);
}
