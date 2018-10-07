#include "installnotice.h"
#include "ui_installnotice.h"

#include <QScrollBar>

InstallNotice::InstallNotice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallNotice) {
    ui->setupUi(this);
    enableAcceptButton();
}

InstallNotice::~InstallNotice() {
    delete ui;
}

void InstallNotice::enableAcceptButton() {
    // Only enable the accept button if the user has scrolled to the bottom
    // of the message.
    if (ui->textBrowser->verticalScrollBar()->value() >= ui->textBrowser->verticalScrollBar()->maximum()) {
        ui->btnAccept->setEnabled(true);
    }
}
