#include <QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "serialhandler.h"

#define TO_PACKET(var_addr) ((char*)&var_addr)
#define TO_DATA(data, type) ((type*)data.data())

class SerialIO {
public:
	SerialIO(const QString portName, int baudRate) {
		init(portName, baudRate, 0, 0);
	}
	SerialIO(const QString portName, int baudRate, readback_t readCallback, writeback_t writeCallback) {
		init(portName, baudRate, readCallback, writeCallback);
	}

	~SerialIO() {
		std::cin.get();
		delete handler;
	}

	void send(QByteArray arr) {
		handler->write(arr);
	}

	void send(uint8_t byte) {
		QByteArray arr;
		arr.resize(1);
		arr[0]=byte;
		handler->write(arr);
	}

	void sendPacket(char * packet_ptr, uint8_t packet_size) {
		QByteArray startCond;
		startCond.resize(2);
		startCond[0]=255;
		startCond[1]=254;
		port.write(startCond);
		port.write(packet_ptr, packet_size);
	}

private:
	void init(const QString portName, int baudRate, readback_t readCallback, writeback_t writeCallback) {
		port.setBaudRate(baudRate);
		port.setPortName(portName);
		port.setDataBits(QSerialPort::Data8);
		port.setStopBits(QSerialPort::OneStop);
		port.setFlowControl(QSerialPort::NoFlowControl);
		port.setParity(QSerialPort::NoParity);
		if(!port.open(QIODevice::ReadWrite)) {
			std::cout<< "Could not open serial port "<< portName.toStdString() << ": " << port.errorString().toStdString()<<std::endl;
			exit(1);
		}

		handler = new SerialHandler(&port, readCallback, writeCallback);
	}

	QSerialPort port;
	SerialHandler * handler;
};

#include <cstdint>
#include <array>

// data that is sent by the computer
struct BluetoothData {
	uint8_t r;
	std::uint8_t messageLength;
	std::array<std::uint8_t, 30> message;
};

void read(QByteArray & data) {
	BluetoothData * d = TO_DATA(data, BluetoothData);
	std::cout<<d->message.data();
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	SerialIO io("COM13", 115200, read, 0);
	std::cout<<"Connected\n";

	BluetoothData a;
	std::string str = "test";
	std::copy(str.begin(),str.end(), a.message.data());
	a.message[str.length()] = 0;
	a.messageLength = str.size();
	a.r = 0;

	io.sendPacket(TO_PACKET(a), sizeof(BluetoothData));

	return app.exec();
}

