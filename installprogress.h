#ifndef INSTALLPROGRESS_H
#define INSTALLPROGRESS_H

#include <QDialog>

namespace Ui {
class InstallProgress;
}

class InstallProgress : public QDialog
{
    Q_OBJECT

public:
    explicit InstallProgress(QWidget *parent = nullptr);
    ~InstallProgress();

private:
    Ui::InstallProgress *ui;
};

#endif // INSTALLPROGRESS_H
