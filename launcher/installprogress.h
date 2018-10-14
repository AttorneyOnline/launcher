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

public slots:
    void installProgress(int progress, const QString &msg = nullptr);
    void subtaskSetup(bool enabled, int max = 100);
    void subtaskProgress(int progress, const QString &msg = nullptr);

private:
    Ui::InstallProgress *ui;
};

#endif // INSTALLPROGRESS_H
