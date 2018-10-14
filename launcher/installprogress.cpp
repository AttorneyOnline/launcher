#include "installprogress.h"
#include "ui_installprogress.h"

#include <QDebug>

InstallProgress::InstallProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallProgress) {
    ui->setupUi(this);
    ui->widgetSubtask->hide();
}

InstallProgress::~InstallProgress() {
    delete ui;
}

void InstallProgress::installProgress(int progress, const QString &msg) {
    ui->progressMainTask->setValue(progress);
    if (msg != nullptr) {
        qInfo() << "install:" << msg;
        ui->labelMainTask->setText(msg);
    }
}

void InstallProgress::subtaskSetup(bool enabled, int max) {
    ui->widgetSubtask->setVisible(enabled);
    ui->progressSubtask->setMaximum(max);
}

void InstallProgress::subtaskProgress(int progress, const QString &msg) {
    ui->progressSubtask->setValue(progress);
    if (msg != nullptr) {
        qInfo() << "subtask:" << msg;
        ui->labelSubtask->setText(msg);
    }
}
