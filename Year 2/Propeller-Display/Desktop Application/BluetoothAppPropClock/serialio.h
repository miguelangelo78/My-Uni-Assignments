#ifndef SERIALIO_H
#define SERIALIO_H
#include <QCoreApplication>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <iostream>
#include <QString>
#include <QtCore>
#include <QInputDialog>
#include "serialhandler.h"

class SerialIO {
 public:
  SerialIO(const QString portName, int baudRate) {
    init(portName, baudRate, 0, 0);
  }
  SerialIO(const QString portName, int baudRate, readback_t readCallback,
           writeback_t writeCallback) {
    init(portName, baudRate, readCallback, writeCallback);
  }

  ~SerialIO() {
    std::cin.get();
    delete handler;
  }
  void send(QByteArray arr) { handler->write(arr); }

 private:
  void init(const QString portName, int baudRate, readback_t readCallback,
            writeback_t writeCallback) {
    port.setBaudRate(baudRate);
    port.setPortName(portName);
    if (!port.open(QIODevice::ReadWrite)) {
      std::cout << "Could not open serial port " << portName.toStdString()
                << ": " << port.errorString().toStdString() << std::endl;

      auto btn = QMessageBox::information(nullptr, "Error: Device doesn't exist!",
                               QObject::tr("device %1 does not exist").arg(portName), QMessageBox::Retry, QMessageBox::Abort);

     if(btn == QMessageBox::Retry) {
         QString deviceName = QInputDialog::getText(nullptr, "enter device name",
                               "please insert device name");

         if(deviceName.size() == 0)
             deviceName = "COM13";

         init(deviceName, baudRate, readCallback, writeCallback);
     }
     else
     exit(1);

    }

    handler = new SerialHandler(&port, readCallback, writeCallback);
  }

  QSerialPort port;
  SerialHandler* handler;
};

#endif  // SERIALIO_H
