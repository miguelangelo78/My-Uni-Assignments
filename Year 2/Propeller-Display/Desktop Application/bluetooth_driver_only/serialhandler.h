#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QByteArray>
#include <QObject>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

typedef void (*readback_t)(QByteArray&);
typedef void (*writeback_t)(qint64);

class SerialHandler : public QObject
{
	Q_OBJECT

public:
	SerialHandler(QSerialPort *port, QObject *parent = 0);
	SerialHandler(QSerialPort *port, readback_t rback, writeback_t wback, QObject *parent = 0);
	void write(const QByteArray & data);
	QByteArray read(void);
	~SerialHandler();

private slots:
	void handleRead();
	void handleWrite(qint64 bytes);
	void handleTimeout();
	void handleError(QSerialPort::SerialPortError error);

private:
	void init();
	readback_t readCallback;
	writeback_t writeCallback;
	QSerialPort *port;
	QByteArray  readBuff;
	QByteArray	writeBuff;
	qint64		writeCount;
	QTimer      timer;
};

#endif // SERIALREADER_H
