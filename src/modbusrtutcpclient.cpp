#include <QUrl>
#include <QDataStream>
#include <QModbusClient>
#include "modbusrtutcpclient.h"
#include "modbusmessage.h"

///
/// \brief ModbusRtuTcpClient::ModbusRtuTcpClient
/// \param parent
///
ModbusRtuTcpClient::ModbusRtuTcpClient(QObject *parent)
    : ModbusClientPrivate(parent)
{
    _socket = new QTcpSocket(this);
    connect(_socket, &QAbstractSocket::connected, this, &ModbusRtuTcpClient::on_connected);
    connect(_socket, &QAbstractSocket::disconnected, this, &ModbusRtuTcpClient::on_disconnected);
    connect(_socket, &QAbstractSocket::errorOccurred, this, &ModbusRtuTcpClient::on_errorOccurred);
    connect(_socket, &QAbstractSocket::readyRead, this, &ModbusRtuTcpClient::on_readyRead);
}

///
/// \brief ModbusRtuTcpClient::~ModbusRtuTcpClient
///
ModbusRtuTcpClient::~ModbusRtuTcpClient()
{
    close();
}

///
/// \brief ModbusRtuTcpClient::open
/// \return
///
bool ModbusRtuTcpClient::open()
{
    if (state() == ModbusDevice::ConnectedState)
        return true;

    if (_socket->state() != QAbstractSocket::UnconnectedState)
        return false;

    const QUrl url = QUrl::fromUserInput(_networkAddress + QStringLiteral(":")
                                         + QString::number(_networkPort));

    if (!url.isValid()) {
        setError(tr("Invalid connection settings for TCP communication specified."), ModbusDevice::ConnectionError);
        qCWarning(QT_MODBUS) << "(TCP client) Invalid host:" << url.host() << "or port:"
                             << url.port();
        return false;
    }

    _socket->connectToHost(url.host(), url.port());

    return true;
}

///
/// \brief ModbusRtuTcpClient::isOpen
/// \return
///
bool ModbusRtuTcpClient::isOpen() const
{
    if (_socket)
        return _socket->isOpen();
    return false;
}

///
/// \brief ModbusRtuTcpClient::close
///
void ModbusRtuTcpClient::close()
{
    if (state() == ModbusDevice::UnconnectedState)
        return;

    _socket->disconnectFromHost();
}

///
/// \brief ModbusRtuTcpClient::connectionParameter
/// \param parameter
/// \return
///
QVariant ModbusRtuTcpClient::connectionParameter(ModbusDevice::ConnectionParameter parameter) const
{
    switch (parameter) {
        case ModbusDevice::NetworkPortParameter:
            return _networkPort;
        case ModbusDevice::NetworkAddressParameter:
            return _networkAddress;
        default:
            break;
    }
    return {};
}

///
/// \brief ModbusRtuTcpClient::setConnectionParameter
/// \param parameter
/// \param value
///
void ModbusRtuTcpClient::setConnectionParameter(ModbusDevice::ConnectionParameter parameter, const QVariant &value)
{
    switch (parameter) {
        case ModbusDevice::NetworkPortParameter: _networkPort = value.toInt(); break;
        case ModbusDevice::NetworkAddressParameter: _networkAddress = value.toString(); break;
        default: Q_ASSERT_X(false, "", "Connection parameter not supported."); break;
    }
}

///
/// \brief ModbusRtuTcpClient::enqueueRequest
/// \param requestGroupId
/// \param request
/// \param serverAddress
/// \param unit
/// \param type
/// \return
///
ModbusReply* ModbusRtuTcpClient::enqueueRequest(int requestGroupId, const QModbusRequest &request, int serverAddress,
                                                const QModbusDataUnit &unit, ModbusReply::ReplyType type)
{
    if (!isOpen() || state() != ModbusDevice::ConnectedState) {
        setError(QModbusClient::tr("Device not connected."), ModbusDevice::ConnectionError);
        return nullptr;
    }

    if (!request.isValid()) {
        setError(QModbusClient::tr("Invalid Modbus request."), ModbusDevice::ProtocolError);
        return nullptr;
    }

    const QByteArray rtuFrame = wrapRtuFrame(request, serverAddress);

    auto reply = new ModbusReply(type, serverAddress, this);
    reply->setRequestGroupId(requestGroupId);
    reply->setTransactionId(0);

    const auto msgReq = ModbusMessage::create(rtuFrame, ModbusMessage::Rtu, QDateTime::currentDateTime(), true);
    emit modbusRequest(requestGroupId, msgReq);

    const QueueElement element{ reply, request, unit, numberOfRetries(), timeout() };
    _transactionStore.insert(0, element);

    if (_socket->write(rtuFrame) != rtuFrame.size()) {
        setError(QModbusClient::tr("Could not write request to socket."), ModbusDevice::WriteError);
        return nullptr;
    }

    incrementTransactionId();
    return reply;
}

///
/// \brief ModbusRtuTcpClient::wrapRtuFrame
/// \param request
/// \param serverAddress
/// \return
///
QByteArray ModbusRtuTcpClient::wrapRtuFrame(const QModbusRequest &request, int serverAddress) const
{
    QByteArray frame;
    QDataStream stream(&frame, QIODevice::WriteOnly);
    stream << quint8(serverAddress) << request;

    // Add CRC16 RTU
    quint16 crc = QModbusAduRtu::calculateCRC(frame.data(), frame.length());
    frame.append((crc >> 8) & 0xFF);
    frame.append(crc & 0xFF);
    return frame;
}

///
/// \brief ModbusRtuTcpClient::unwrapRtuResponse
/// \param frame
/// \param serverAddress
/// \return
///
QModbusResponse ModbusRtuTcpClient::unwrapRtuResponse(const QByteArray &frame, int serverAddress)
{
    if (frame.size() < 5) // minimum: addr+func+crc
        return QModbusResponse();

    quint16 crcReceived = (quint8(frame.at(frame.size() - 2)) << 8) | quint8(frame.at(frame.size() - 1));
    QByteArray pduData = frame.left(frame.size() - 2);
    quint16 crcCalc = QModbusAduRtu::calculateCRC(pduData.data(), pduData.length());

    if (crcReceived != crcCalc)
        return QModbusResponse(); // CRC error

    QDataStream stream(pduData);
    quint8 addr;
    stream >> addr;
    if (addr != serverAddress)
        return QModbusResponse(); // invalid server address

    QModbusResponse response;
    stream >> response;
    return response;
}

///
/// \brief ModbusRtuTcpClient::on_connected
///
void ModbusRtuTcpClient::on_connected()
{
    qCDebug(QT_MODBUS) << "(TCP client) Connected to" << _socket->peerAddress() << "on port" << _socket->peerPort();
    _responseBuffer.clear();
    setState(ModbusDevice::ConnectedState);
}

///
/// \brief ModbusRtuTcpClient::on_disconnected
///
void ModbusRtuTcpClient::on_disconnected()
{
    qCDebug(QT_MODBUS)  << "(TCP client) Connection closed.";
    setState(ModbusDevice::UnconnectedState);
    cleanupTransactionStore();
}

///
/// \brief ModbusRtuTcpClient::on_errorOccurred
///
void ModbusRtuTcpClient::on_errorOccurred(QAbstractSocket::SocketError)
{
    if (_socket->state() == QAbstractSocket::UnconnectedState) {
        cleanupTransactionStore();
        setState(ModbusDevice::UnconnectedState);
    }
    setError(QModbusClient::tr("TCP socket error (%1).").arg(_socket->errorString()), ModbusDevice::ConnectionError);
}

///
/// \brief ModbusRtuTcpClient::on_readyRead
///
void ModbusRtuTcpClient::on_readyRead()
{
    _responseBuffer += _socket->readAll();

    while (_responseBuffer.size() >= 5) {
        quint8 serverAddr = quint8(_responseBuffer.at(0));
        QModbusResponse resp = unwrapRtuResponse(_responseBuffer, serverAddr);

        if (!resp.isValid())
            return;

        if (_transactionStore.contains(0))
            processQueueElement(resp, ModbusMessage::Rtu, serverAddr, _transactionStore[0]);

        _responseBuffer.remove(0, resp.size() + 3);
    }
}

///
/// \brief ModbusRtuTcpClient::cleanupTransactionStore
///
void ModbusRtuTcpClient::cleanupTransactionStore()
{
    for (auto &elem : _transactionStore) {
        if (!elem.reply.isNull())
            elem.reply->setError(ModbusDevice::ReplyAbortedError,
                                 QModbusClient::tr("Reply aborted due to connection closure."));
    }
    _transactionStore.clear();
}
