#ifndef MODBUSTCPCLIENT_H
#define MODBUSTCPCLIENT_H

#include <QTcpSocket>
#include "modbusclientprivate.h"

///
/// \brief The ModbusTcpClient class
///
class ModbusTcpClient : public ModbusClientPrivate
{
    Q_OBJECT
public:
    explicit ModbusTcpClient(QObject *parent = nullptr);
    ~ModbusTcpClient();

    QVariant connectionParameter(ModbusDevice::ConnectionParameter parameter) const override;
    void setConnectionParameter(ModbusDevice::ConnectionParameter parameter, const QVariant &value) override;

    QIODevice *device() const override {return _socket; }

    bool isOpen() const override;

protected:
    bool open() override;
    void close() override;

    ModbusReply* enqueueRequest(int requestGroupId, const QModbusRequest &request, int serverAddress, const QModbusDataUnit &unit, ModbusReply::ReplyType type) override;

private slots:
    void on_connected();
    void on_disconnected();
    void on_errorOccurred(QAbstractSocket::SocketError error);
    void on_readyRead();

private:
    void cleanupTransactionStore();
    inline void incrementTransactionId() { _transactionId++; }
    inline int transactionId() const { return _transactionId; }

private:
    int _networkPort = 502;
    QString _networkAddress = QStringLiteral("127.0.0.1");

    QTcpSocket *_socket = nullptr;
    QByteArray _responseBuffer;
    QHash<quint16, QueueElement> _transactionStore;
    int mbpaHeaderSize = 7;
    quint16 _transactionId = 0;
};

#endif // MODBUSTCPCLIENT_H
