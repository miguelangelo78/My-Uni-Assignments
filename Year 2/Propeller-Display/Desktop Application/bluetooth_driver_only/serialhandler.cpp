#include "serialhandler.h"

#include <QCoreApplication>
#include <iostream>

QT_USE_NAMESPACE

#define TIMER_START_DELAY 10

void SerialHandler::init()
{
	/* Install function handlers: */
	connect(port, SIGNAL(readyRead()), SLOT(handleRead()));
	connect(port, SIGNAL(bytesWritten(qint64)), SLOT(handleWrite(qint64)));
	connect(port, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
	connect(&timer, SIGNAL(timeout()), SLOT(handleTimeout()));

	/* Start timer: */
	timer.setSingleShot(true);
	timer.start(TIMER_START_DELAY);
}

SerialHandler::SerialHandler(QSerialPort *port, QObject *parent)
	: QObject(parent)
	, port(port)
	, writeCount(0)
	, readCallback(0)
	, writeCallback(0)
{
	init();
}

SerialHandler::SerialHandler(QSerialPort *port, readback_t rback, writeback_t wback, QObject *parent)
	: QObject(parent)
	, port(port)
	, writeCount(0)
	, readCallback(rback)
	, writeCallback(wback)
{
	init();
}

SerialHandler::~SerialHandler() {}

void SerialHandler::write(const QByteArray & data)
{
	writeBuff = data;

	qint64 bytesWritten = port->write(data);
	if(bytesWritten == -1) {
		std::cout<<"Failed to write the data: "<<port->errorString().toStdString()<<std::endl;
		exit(1);
	} else if(bytesWritten != writeBuff.size()) {
		std::cout<<"Failed to write all the data: "<<port->errorString().toStdString() <<std::endl;
		exit(1);
	}
}

QByteArray SerialHandler::read(void)
{
	return readBuff;
}

void SerialHandler::handleRead()
{
	readBuff.append(port->readAll());

	if (!timer.isActive())
		timer.start(TIMER_START_DELAY);
}

void SerialHandler::handleWrite(qint64 bytes)
{
	if((writeCount+=bytes) == writeBuff.size()) {
		if(writeCallback)
			writeCallback(writeCount);
		writeCount = 0;
		/* Sucess */
	}
}

void SerialHandler::handleTimeout()
{
	if (!readBuff.isEmpty()) {
		if(!readCallback)
			std::cout << readBuff.toStdString();
		else
			readCallback(readBuff);
		readBuff.clear();
	}
}

void SerialHandler::handleError(QSerialPort::SerialPortError error)
{
	if (error == QSerialPort::ReadError) {
		std::cout<< "ERROR: IO error - " << port->errorString().toStdString() << std::endl;
		QCoreApplication::exit(1);
	}
}
