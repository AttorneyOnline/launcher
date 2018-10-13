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
    Ui::MainWindow *ui;

    QDir installPath;
    QString versionFilePath;

    const std::vector<QString> packages = {"program", "assets"};
    std::map<QString, Updater> updaters;

    void refresh();

private slots:
    void play();
    void install();
    void checkForUpdates(bool manual = true);
    void showOptions();
};

#endif // MAINWINDOW_H
