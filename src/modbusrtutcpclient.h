#ifndef MODBUSRTUTCPCLIENT_H
#define MODBUSRTUTCPCLIENT_H

#include <QQueue>
#include <QTcpSocket>
#include "modbusclientprivate.h"

///
/// \brief The ModbusRtuTcpClient class
/// This class implements Modbus RTU over TCP.
///
class ModbusRtuTcpClient : public ModbusClientPrivate
{
    Q_OBJECT
public:
    explicit ModbusRtuTcpClient(QObject *parent = nullptr);
    ~ModbusRtuTcpClient();

    int interFrameDelay() const;
    void setInterFrameDelay(int microseconds);

    QVariant connectionParameter(ModbusDevice::ConnectionParameter parameter) const override;
    void setConnectionParameter(ModbusDevice::ConnectionParameter parameter, const QVariant &value) override;

    QIODevice *device() const override { return _socket; }
    bool isOpen() const override;

protected:
    bool open() override;
    void close() override;
    ModbusReply* enqueueRequest(int requestGroupId, const QModbusRequest &request, int serverAddress,
                                const QModbusDataUnit &unit, ModbusReply::ReplyType type) override;

private slots:
    void on_connected();
    void on_disconnected();
    void on_errorOccurred(QAbstractSocket::SocketError error);
    void on_readyRead();

private:
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

    int _networkPort = 502;
    QString _networkAddress = QStringLiteral("127.0.0.1");

    QTimer _responseTimer;
    QTcpSocket *_socket = nullptr;
    QByteArray _responseBuffer;
    QQueue<QueueElement> _queue;

    static constexpr int RecommendedDelay = 2; // A approximated value of 1.750 msec.
    int _interFrameDelayMilliseconds = RecommendedDelay;
};

#endif // MODBUSRTUTCPCLIENT_H
