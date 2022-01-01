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
    ~Options() override;

    static const std::map<const QString, const QVariant> defaultOptions;

    template <typename T>
    static T getOption(const QSettings &settings, const QString &option);

    static QString getRepositoryUrl(const QSettings &settings, const QString &repo);
private:
    QScopedPointer<Ui::Options> ui;

private slots:
    void on_btnBrowseInstallPath_clicked();
    void accept() override;
};

#endif // OPTIONS_H
