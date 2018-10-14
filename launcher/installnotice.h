#ifndef INSTALLNOTICE_H
#define INSTALLNOTICE_H

#include <QDialog>

namespace Ui {
class InstallNotice;
}

class InstallNotice : public QDialog
{
    Q_OBJECT

public:
    explicit InstallNotice(QWidget *parent = nullptr);
    ~InstallNotice();

private:
    Ui::InstallNotice *ui;

private slots:
    void enableAcceptButton();
};

#endif // INSTALLNOTICE_H
