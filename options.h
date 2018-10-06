#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = nullptr);
    ~Options();

private:
    Ui::Options *ui;

private slots:
    void on_btnBrowseInstallPath_clicked();
    void accept();
};

#endif // OPTIONS_H
