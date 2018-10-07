#include "installprogress.h"
#include "ui_installprogress.h"

InstallProgress::InstallProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallProgress)
{
    ui->setupUi(this);
    ui->widgetSubtask->hide();
}

InstallProgress::~InstallProgress()
{
    delete ui;
}
