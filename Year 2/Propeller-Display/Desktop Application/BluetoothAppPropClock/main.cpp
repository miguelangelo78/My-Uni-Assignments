#include "mainwindow.h"
#include <QApplication>
#include <QInputDialog>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QString deviceName = QInputDialog::getText(nullptr, "enter device name",
                          "please insert device name");

    if(deviceName.size() == 0)
        deviceName = "COM13";

    MainWindow w(deviceName);
    w.show();

    return a.exec();
}
