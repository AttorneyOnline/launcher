#include "options.h"
#include "ui_options.h"

#include <QFileDialog>
#include <QSettings>
#include <QtDebug>

const std::map<const QString, const QVariant> Options::defaultOptions = {
    {"path", "client"},
    {"repos/program", "https://server.raidensnakesden.net/program_${os}_${arch}.json"},
    {"repos/assets", "https://server.raidensnakesden.net/assets.json"},
    {"checkOnLaunch", Qt::CheckState::Checked},
    {"version", APP_VERSION}
};

template <typename T>
const T Options::getOption(const QSettings &settings, const QString &option) {
    return settings.value(option, defaultOptions.at(option)).value<T>();
}

template const QString Options::getOption<QString>(const QSettings &settings, const QString &option);

const QString Options::getRepositoryUrl(const QSettings &settings, const QString &repo) {
    auto repoUrl = getOption<QString>(settings, "repos/" + repo);

    // OSes of interest: winnt, darwin, linux
    repoUrl.replace("${os}", QSysInfo::kernelType());

    // Architectures of interest: i386, x86_64
    repoUrl.replace("${arch}", QSysInfo::currentCpuArchitecture());

    return repoUrl;
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
        if (option.second == "") {
            settings.remove(option.first);
        } else {
            settings.setValue(option.first, option.second);
        }
    }
    qDebug() << "Wrote settings to" << settings.fileName();
    done(QDialog::Accepted);
}
