#ifndef OPTIONS_H
#define OPTIONS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class Options;
}

class Options : public QDialog
{
    Q_OBJECT

public:
    explicit Options(QWidget *parent = nullptr);
    ~Options();

    static const std::map<const QString, const QVariant> defaultOptions;

    template <class T>
    static T getOption (QSettings &settings, const QString &option);

private:
    Ui::Options *ui;

private slots:
    void on_btnBrowseInstallPath_clicked();
    void accept();
};

#endif // OPTIONS_H
