#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "updater.h"

#include <QMainWindow>
#include <QDir>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static QString getVersionFilePath(const QSettings &settings);

private:
    const QString TROUBLESHOOT_MSG = tr("<b>For network errors:</b><br>"
                                        "Please ensure that your connection is not blocked by a firewall. "
                                        "If you modified a repository path, please ensure that it is correct.");

    Ui::MainWindow *ui;

    QDir installPath;
    QString versionFilePath;

    QByteArray branding;

    const std::vector<QString> packages = {"program", "assets"};

    void refresh();

private slots:
    void play();
    void install();
    void checkForUpdates(bool manual = true);
    void showOptions();
};

#endif // MAINWINDOW_H
