#ifndef MODBUSRTUCLIENT_H
#define MODBUSRTUCLIENT_H

#include <QQueue>
#include <QTimerEvent>
#include <QSerialPort>
#include "modbusclientprivate.h"

///
/// \brief The Timer class
///
class Timer : public QObject
{
    Q_OBJECT

public:
    Timer() = default;
    int start(int msec)
    {
        m_timer = QBasicTimer();
        m_timer.start(msec, Qt::PreciseTimer, this);
        return m_timer.timerId();
    }
    void stop() { m_timer.stop(); }
    bool isActive() const { return m_timer.isActive(); }

signals:
    void timeout(int timerId);

private:
    void timerEvent(QTimerEvent *event) override
    {
        const auto id = m_timer.timerId();
        if (event->timerId() == id)
            emit timeout(id);
    }

private:
    QBasicTimer m_timer;
};

///
/// \brief The ModbusRtuClient class
///
class ModbusRtuClient : public ModbusClientPrivate
{
    Q_OBJECT
public:
    explicit ModbusRtuClient(QObject *parent = nullptr);
    ~ModbusRtuClient();

    int interFrameDelay() const;
    void setInterFrameDelay(int microseconds);

    int turnaroundDelay() const;
    void setTurnaroundDelay(int turnaroundDelay);

    QVariant connectionParameter(ModbusDevice::ConnectionParameter parameter) const override;
    void setConnectionParameter(ModbusDevice::ConnectionParameter parameter, const QVariant &value) override;

    QIODevice *device() const override { return _serialPort; }

    bool isOpen() const override;

protected:
    bool open() override;
    void close() override;

    ModbusReply* enqueueRequest(int requestGroupId, const QModbusRequest &request, int serverAddress, const QModbusDataUnit &unit, ModbusReply::ReplyType type) override;

private slots:
    void on_readyRead();
    void on_aboutToClose();
    void on_responseTimeout(int timerId);
    void on_bytesWritten(qint64 bytes);
    void on_error(QSerialPort::SerialPortError error);

private:
    void setupEnvironment();
    void calculateInterFrameDelay();
    void scheduleNextRequest(int delay);
    void processQueue();
    bool canMatchRequestAndResponse(const QModbusResponse &response, int sendingServer) const;

private:
    enum State
    {
        Idle,
        WaitingForReplay,
        ProcessReply
    } _state = Idle;

    QString _comPort;
    QSerialPort::DataBits _dataBits = QSerialPort::Data8;
    QSerialPort::Parity _parity = QSerialPort::EvenParity;
    QSerialPort::StopBits _stopBits = QSerialPort::OneStop;
    QSerialPort::BaudRate _baudRate = QSerialPort::Baud19200;

    QSerialPort* _serialPort = nullptr;
    Timer _responseTimer;
    QByteArray _responseBuffer;
    QQueue<QueueElement> _queue;

    static constexpr int RecommendedDelay = 2; // A approximated value of 1.750 msec.
    int _interFrameDelayMilliseconds = RecommendedDelay;

    int _turnaroundDelay = 100; // Recommended value is between 100 and 200 msec.
};

#endif // MODBUSRTUCLIENT_H
