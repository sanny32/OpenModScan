#include <QUrl>
#include <QHostAddress>
#include <QModbusClient>
#include <QModbusTcpClient>
#include "modbustcpclient.h"

///
/// \brief ModbusTcpClient::ModbusTcpClient
/// \param parent
///
ModbusTcpClient::ModbusTcpClient(QObject *parent)
{
    _socket = new QTcpSocket(this);
    QObject::connect(_socket, &QAbstractSocket::connected, this, &ModbusTcpClient::on_connected);
    QObject::connect(_socket, &QAbstractSocket::disconnected, this, &ModbusTcpClient::on_disconnected);
    QObject::connect(_socket, &QAbstractSocket::errorOccurred, this, &ModbusTcpClient::on_errorOccurred);
    QObject::connect(_socket, &QAbstractSocket::readyRead, this, &ModbusTcpClient::on_readyRead);
}

///
/// \brief ModbusTcpClient::~ModbusTcpClient
///
ModbusTcpClient::~ModbusTcpClient()
{
    close();
}

///
/// \brief ModbusTcpClient::open
/// \return
///
bool ModbusTcpClient::open()
{
    if (state() == QModbusDevice::ConnectedState)
        return true;

    if (_socket->state() != QAbstractSocket::UnconnectedState)
        return false;

    const QUrl url = QUrl::fromUserInput(_networkAddress + QStringLiteral(":")
                                         + QString::number(_networkPort));

    if (!url.isValid()) {
        setError(tr("Invalid connection settings for TCP communication specified."), QModbusDevice::ConnectionError);
        qCWarning(QT_MODBUS) << "(TCP client) Invalid host:" << url.host() << "or port:"
                             << url.port();
        return false;
    }

    _socket->connectToHost(url.host(), url.port());

    return true;
}

///
/// \brief ModbusTcpClient::isOpen
/// \return
///
bool ModbusTcpClient::isOpen() const
{
    if (_socket)
        return _socket->isOpen();
    return false;
}

///
/// \brief ModbusTcpClient::close
///
void ModbusTcpClient::close()
{
    if (state() == QModbusDevice::UnconnectedState)
        return;

    _socket->disconnectFromHost();
}

///
/// \brief ModbusTcpClient::connectionParameter
/// \param parameter
/// \return
///
QVariant ModbusTcpClient::connectionParameter(QModbusDevice::ConnectionParameter parameter) const
{
    switch (parameter) {
    case QModbusDevice::NetworkPortParameter:
        return _networkPort;
    case QModbusDevice::NetworkAddressParameter:
        return _networkAddress;
    default:
        break;
    }
    return {};
}

///
/// \brief ModbusTcpClient::setConnectionParameter
/// \param parameter
/// \param value
///
void ModbusTcpClient::setConnectionParameter(QModbusDevice::ConnectionParameter parameter, const QVariant &value)
{
    switch (parameter) {
    case QModbusDevice::NetworkPortParameter:
        _networkPort = value.toInt();
        break;
    case QModbusDevice::NetworkAddressParameter:
        _networkAddress = value.toString();
        break;
    default:
        Q_ASSERT_X(false, "", "Connection parameter not supported.");
        break;
    }
}

///
/// \brief ModbusTcpClient::on_connected
///
void ModbusTcpClient::on_connected()
{
    qCDebug(QT_MODBUS) << "(TCP client) Connected to" << _socket->peerAddress() << "on port" << _socket->peerPort();
    _responseBuffer.clear();
    setState(QModbusDevice::ConnectedState);
}

///
/// \brief ModbusTcpClient::on_disconnected
///
void ModbusTcpClient::on_disconnected()
{
    qCDebug(QT_MODBUS)  << "(TCP client) Connection closed.";
    setState(QModbusDevice::UnconnectedState);
    cleanupTransactionStore();
}

///
/// \brief ModbusTcpClient::on_errorOccurred
/// \param error
///
void ModbusTcpClient::on_errorOccurred(QAbstractSocket::SocketError error)
{
    if (_socket->state() == QAbstractSocket::UnconnectedState) {
        cleanupTransactionStore();
        setState(QModbusDevice::UnconnectedState);
    }
    setError(QModbusClient::tr("TCP socket error (%1).").arg(_socket->errorString()), QModbusDevice::ConnectionError);
}

///
/// \brief ModbusTcpClient::on_readyRead
///
void ModbusTcpClient::on_readyRead()
{
    _responseBuffer += _socket->read(_socket->bytesAvailable());
    qCDebug(QT_MODBUS_LOW) << "(TCP client) Response buffer:" << _responseBuffer.toHex();

    while (!_responseBuffer.isEmpty()) {
        // can we read enough for Modbus ADU header?
        if (_responseBuffer.size() < mbpaHeaderSize) {
            qCDebug(QT_MODBUS_LOW) << "(TCP client) MBPA header too short. Waiting for more data.";
            return;
        }

        quint8 serverAddress;
        quint16 transactionId, bytesPdu, protocolId;
        QDataStream input(_responseBuffer);
        input >> transactionId >> protocolId >> bytesPdu >> serverAddress;

        // stop the timer as soon as we know enough about the transaction
        const bool knownTransaction = _transactionStore.contains(transactionId);
        if (knownTransaction && _transactionStore[transactionId].timer)
            _transactionStore[transactionId].timer->stop();

        qCDebug(QT_MODBUS) << "(TCP client) tid:" << Qt::hex << transactionId << "size:"
                           << bytesPdu << "server address:" << serverAddress;

        // The length field is the byte count of the following fields, including the Unit
        // Identifier and the PDU, so we remove on byte.
        bytesPdu--;

        int tcpAduSize = mbpaHeaderSize + bytesPdu;
        if (_responseBuffer.size() < tcpAduSize) {
            qCDebug(QT_MODBUS) << "(TCP client) PDU too short. Waiting for more data";
            return;
        }

        QModbusResponse responsePdu;
        input >> responsePdu;
        qCDebug(QT_MODBUS) << "(TCP client) Received PDU:" << responsePdu.functionCode()
                           << responsePdu.data().toHex();

        _responseBuffer.remove(0, tcpAduSize);

        if (!knownTransaction) {
            qCDebug(QT_MODBUS) << "(TCP client) No pending request for response with "
                                  "given transaction ID, ignoring response message.";
        } else {
            processQueueElement(responsePdu, ModbusMessage::Tcp, serverAddress, _transactionStore[transactionId]);
        }
    }
}

///
/// \brief ModbusTcpClient::enqueueRequest
/// \param request
/// \param serverAddress
/// \param unit
/// \param type
/// \return
///
QModbusReply* ModbusTcpClient::enqueueRequest(int requestGroupId, const QModbusRequest& request, int serverAddress, const QModbusDataUnit& unit, QModbusReply::ReplyType type)
{
    auto writeToSocket = [this](int requestGroupId, quint16 tId, const QModbusRequest& request, int address) {
        QByteArray buffer;
        QDataStream output(&buffer, QIODevice::WriteOnly);
        output << tId << quint16(0) << quint16(request.size() + 1) << quint8(address) << request;

        const auto msgReq = ModbusMessage::create(buffer, ModbusMessage::Tcp, QDateTime::currentDateTime(), true);
        emit modbusRequest(requestGroupId, msgReq);

        int writtenBytes = _socket->write(buffer);
        if (writtenBytes == -1 || writtenBytes < buffer.size()) {
            qCDebug(QT_MODBUS) << "(TCP client) Cannot write request to socket.";
            setError(QModbusTcpClient::tr("Could not write request to socket."), QModbusDevice::WriteError);
            return false;
        }
        qCDebug(QT_MODBUS_LOW) << "(TCP client) Sent TCP ADU:" << buffer.toHex();
        qCDebug(QT_MODBUS) << "(TCP client) Sent TCP PDU:" << request << "with tId:" <<Qt:: hex
                           << tId;
        return true;
    };

    const int tId = transactionId();
    if (!writeToSocket(requestGroupId, tId, request, serverAddress))
        return nullptr;

    auto reply = new QModbusReply(type, serverAddress, this);
    reply->setProperty("RequestGroupId", requestGroupId);
    reply->setProperty("TransactionId", tId);

    const auto element = QueueElement{ reply, request, unit, numberOfRetries(), timeout() };
    _transactionStore.insert(tId, element);

    connect(reply, &QObject::destroyed, this, [this, tId](QObject *) {
        if (!_transactionStore.contains(tId))
            return;
        const QueueElement element = _transactionStore.take(tId);
        if (element.timer)
            element.timer->stop();
    });

    if (element.timer) {
        connect(this, &ModbusTcpClient::timeoutChanged, element.timer.data(), QOverload<int>::of(&QTimer::setInterval));
        QObject::connect(element.timer.data(), &QTimer::timeout, this, [this, requestGroupId, writeToSocket, tId]() {
            if (!_transactionStore.contains(tId))
                return;

            QueueElement elem = _transactionStore.take(tId);
            if (elem.reply.isNull())
                return;

            if (elem.numberOfRetries > 0) {
                elem.numberOfRetries--;
                if (!writeToSocket(requestGroupId, tId, elem.requestPdu, elem.reply->serverAddress()))
                    return;
                _transactionStore.insert(tId, elem);
                elem.timer->start();
                qCDebug(QT_MODBUS) << "(TCP client) Resend request with tId:" << Qt::hex << tId;
            } else {
                qCDebug(QT_MODBUS) << "(TCP client) Timeout of request with tId:" <<Qt::hex << tId;
                elem.reply->setError(QModbusDevice::TimeoutError,
                                     QModbusClient::tr("Request timeout."));
            }
        });
        element.timer->start();
    } else {
        qCWarning(QT_MODBUS) << "(TCP client) No response timeout timer for request with tId:"
                             << Qt::hex << tId << ". Expected timeout:" << timeout();
    }
    incrementTransactionId();

    return reply;
}

///
/// \brief ModbusTcpClient::cleanupTransactionStore
///
void ModbusTcpClient::cleanupTransactionStore()
{
    if (_transactionStore.isEmpty())
        return;

    qCDebug(QT_MODBUS) << "(TCP client) Cleanup of pending requests";

    for (const auto &elem : std::as_const(_transactionStore)) {
        if (elem.reply.isNull())
            continue;
        elem.reply->setError(QModbusDevice::ReplyAbortedError,
                             QModbusClient::tr("Reply aborted due to connection closure."));
    }
    _transactionStore.clear();
}
