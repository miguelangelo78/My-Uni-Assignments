#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialio.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString deviceName, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void sendBluetooth();

private:
    Ui::MainWindow *ui;
    SerialIO serialIO;
};

#endif // MAINWINDOW_H
