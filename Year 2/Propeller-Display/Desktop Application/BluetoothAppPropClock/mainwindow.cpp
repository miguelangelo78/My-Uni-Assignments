#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QMessageBox>
#include <QtBluetooth>
#include <cstdint>
#include <array>

void read(QByteArray& data) {
  uint16_t status = (uint16_t)(data[0]) | (uint16_t)(data[1]) << 8;

  if (status == 0) {
    QMessageBox::information(nullptr, "Error: Transmission Failed",
                             "transmission could not be completed",
                             QMessageBox::Ok);
  }
}

MainWindow::MainWindow(QString deviceName, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), serialIO(deviceName, 115200, read, nullptr) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

enum class RequestedDrawMode : std::uint8_t { clockMode, messageMode };

struct BluetoothData {
  RequestedDrawMode requestedDrawMode;
  std::uint8_t messageLength;
  std::array<std::uint8_t, 23> message;
};

void MainWindow::sendBluetooth() {
  BluetoothData bd;
  //
  QString strOptions = ui->comboBox->currentText();

  if (strOptions == "Clock Mode") {
    bd.requestedDrawMode = RequestedDrawMode::clockMode;
  } else {
    std::string str = ui->lineEdit->text().toStdString();

    if (str.size() > 8) {
      QMessageBox::information(
          this, tr("too many characters!"),
          tr("message must be less than or equal to 8 characters"),
          QMessageBox::Ok);

      return;
    }

    std::copy(str.data(), str.data() + str.size(), bd.message.begin());

    bd.messageLength = str.size();

    bd.requestedDrawMode = RequestedDrawMode::messageMode;
  }

  QByteArray buf;
  buf.resize(sizeof(BluetoothData));

  memcpy((void *)buf.data(), (void *)&bd, sizeof(BluetoothData));

  serialIO.send(buf);
}
