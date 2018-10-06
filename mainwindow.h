#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QDir installPath;
    QString versionFilePath;

    void refresh();

private slots:
    void play();
    void install();
    void checkForUpdates(bool manual = true);
    void showOptions();
};

#endif // MAINWINDOW_H
