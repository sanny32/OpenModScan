#include <QUrl>
#include <QtMath>
#include <QDataStream>
#include <QHostAddress>
#include <QModbusClient>
#include "qmodbusserialadu.h"
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
    connect(&_responseTimer, &QObjectTimer::timeout, this, &ModbusRtuTcpClient::on_responseTimeout);
    connect(_socket, &QAbstractSocket::connected, this, &ModbusRtuTcpClient::on_connected);
    connect(_socket, &QAbstractSocket::disconnected, this, &ModbusRtuTcpClient::on_disconnected);
    connect(_socket, &QAbstractSocket::errorOccurred, this, &ModbusRtuTcpClient::on_errorOccurred);
    connect(_socket, &QAbstractSocket::stateChanged, this, &ModbusRtuTcpClient::on_stateChanged);
    connect(_socket, &QAbstractSocket::readyRead, this, &ModbusRtuTcpClient::on_readyRead);
    connect(_socket, &QAbstractSocket::bytesWritten, this, &ModbusRtuTcpClient::on_bytesWritten);
}

///
/// \brief ModbusRtuTcpClient::~ModbusRtuTcpClient
///
ModbusRtuTcpClient::~ModbusRtuTcpClient()
{
    ModbusRtuTcpClient::close();
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
        qCWarning(QT_MODBUS) << "(RTU over TCP client) Invalid host:" << url.host() << "or port:"
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

    int numberOfAborts = 0;
    while (!_queue.isEmpty()) {
        // Finish each open reply and forget them
        ModbusClientPrivate::QueueElement elem = _queue.dequeue();
        if (!elem.reply.isNull()) {
            elem.reply->setError(ModbusDevice::ReplyAbortedError,
                                 QModbusClient::tr("Reply aborted due to connection closure."));
            numberOfAborts++;
        }
    }

    if (numberOfAborts > 0)
        qCDebug(QT_MODBUS_LOW) << "(RTU over TCP client) Aborted replies:" << numberOfAborts;
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
/// \brief ModbusRtuTcpClient::interFrameDelay
/// \return
///
int ModbusRtuTcpClient::interFrameDelay() const
{
    return _interFrameDelayMilliseconds * 1000;
}

///
/// \brief ModbusRtuTcpClient::setInterFrameDelay
/// \param microseconds
///
void ModbusRtuTcpClient::setInterFrameDelay(int microseconds)
{
    _interFrameDelayMilliseconds = qCeil(qreal(microseconds) / 1000.);
}

///
/// \brief ModbusRtuTcpClient::turnaroundDelay
/// \return
///
int ModbusRtuTcpClient::turnaroundDelay() const
{
    return _turnaroundDelay;
}

///
/// \brief ModbusRtuTcpClient::setTurnaroundDelay
/// \param turnaroundDelay
///
void ModbusRtuTcpClient::setTurnaroundDelay(int turnaroundDelay)
{
    _turnaroundDelay = turnaroundDelay;
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
    auto reply = new ModbusReply(serverAddress == 0 ? ModbusReply::Broadcast : type, serverAddress, this);
    reply->setRequestGroupId(requestGroupId);

    QueueElement element(reply, request, unit, numberOfRetries() + 1);
    element.adu = QModbusSerialAdu::create(QModbusSerialAdu::Rtu, serverAddress, request);
    _queue.enqueue(element);

    scheduleNextRequest(_interFrameDelayMilliseconds);

    return reply;
}

///
/// \brief ModbusRtuTcpClient::on_connected
///
void ModbusRtuTcpClient::on_connected()
{
    qCDebug(QT_MODBUS) << "(RTU over TCP client) Connected to" << _socket->peerAddress() << "on port" << _socket->peerPort();
    _responseBuffer.clear();
    setState(ModbusDevice::ConnectedState);
}

///
/// \brief ModbusRtuTcpClient::on_disconnected
///
void ModbusRtuTcpClient::on_disconnected()
{
    qCDebug(QT_MODBUS)  << "(RTU over TCP client) Connection closed.";
    setState(ModbusDevice::UnconnectedState);
}

///
/// \brief ModbusRtuTcpClient::on_errorOccurred
///
void ModbusRtuTcpClient::on_errorOccurred(QAbstractSocket::SocketError)
{
    if (_socket->state() == QAbstractSocket::UnconnectedState) {
        setState(ModbusDevice::UnconnectedState);
    }
    setError(QModbusClient::tr("TCP socket error (%1).").arg(_socket->errorString()), ModbusDevice::ConnectionError);
}

///
/// \brief ModbusRtuTcpClient::on_stateChanged
/// \param state
///
void ModbusRtuTcpClient::on_stateChanged(QAbstractSocket::SocketState state)
{
    if(state == QAbstractSocket::ClosingState) {
        _responseTimer.stop();
    }
}

///
/// \brief ModbusRtuTcpClient::scheduleNextRequest
/// \param delay
///
void ModbusRtuTcpClient::scheduleNextRequest(int delay)
{
    if (_state == Idle && !_queue.isEmpty()) {
        _state = WaitingForReplay;
        QTimer::singleShot(delay, this, [this]() { processQueue(); });
    }
}

///
/// \brief ModbusRtuTcpClient::processQueue
///
void ModbusRtuTcpClient::processQueue()
{
    _responseBuffer.clear();

    if (_queue.isEmpty())
        return;
    auto &current = _queue.first();

    if (current.reply.isNull()) {
        _queue.dequeue();
        _state = Idle;
        scheduleNextRequest(_interFrameDelayMilliseconds);
    } else {

        const auto msgReq = ModbusMessage::create(current.adu, ModbusMessage::Rtu, QDateTime::currentDateTime(), true);
        emit modbusRequest(current.reply->requestGroupId(), msgReq);

        current.bytesWritten = 0;
        current.numberOfRetries--;

        const auto writtenBytes = _socket->write(current.adu);
        if (writtenBytes == -1 || writtenBytes < current.adu.size()) {
            qCDebug(QT_MODBUS) << "(TCP client) Cannot write request to socket.";
            setError(QModbusClient::tr("Could not write request to socket."), ModbusDevice::WriteError);
            return;
        }

        qCDebug(QT_MODBUS) << "(RTU over TCP client) Sent Serial PDU:" << current.requestPdu;
        qCDebug(QT_MODBUS_LOW).noquote() << "(RTU over TCP client) Sent Serial ADU: 0x" + current.adu.toHex();
    }
}

///
/// \brief ModbusRtuTcpClient::canMatchRequestAndResponse
/// \param response
/// \param sendingServer
/// \return
///
bool ModbusRtuTcpClient::canMatchRequestAndResponse(const QModbusResponse &response, int sendingServer) const
{
    if (_queue.isEmpty())
        return false;
    const auto &current = _queue.first();

    if (current.reply.isNull())
        return false;   // reply deleted
    if (current.reply->serverAddress() != sendingServer)
        return false;   // server mismatch
    if (current.requestPdu.functionCode() != response.functionCode())
        return false;   // request for different function code
    return true;
}

///
/// \brief ModbusRtuTcpClient::on_readyRead
///
void ModbusRtuTcpClient::on_readyRead()
{
    _responseBuffer += _socket->readAll();
    qCDebug(QT_MODBUS_LOW) << "(RTU over TCP client) Response buffer:" << _responseBuffer.toHex();

    if (_responseBuffer.size() < 2) {
        qCDebug(QT_MODBUS) << "(RTU over TCP client) Modbus ADU not complete";
        return;
    }

    const QModbusSerialAdu tmpAdu(QModbusSerialAdu::Rtu, _responseBuffer);
    int pduSizeWithoutFcode = QModbusResponse::calculateDataSize(tmpAdu.pdu());
    if (pduSizeWithoutFcode < 0) {
        // wait for more data
        qCDebug(QT_MODBUS) << "(RTU over TCP client) Cannot calculate PDU size for function code:"
                           << tmpAdu.pdu().functionCode() << ", delaying pending frame";
        return;
    }

    // server address byte + function code byte + PDU size + 2 bytes CRC
    int aduSize = 2 + pduSizeWithoutFcode + 2;
    if (tmpAdu.rawSize() < aduSize) {
        qCDebug(QT_MODBUS) << "(RTU over TCP client) Incomplete ADU received, ignoring";

        if (!_queue.isEmpty() && !_queue.first().reply.isNull()) {
            const auto msg = ModbusMessage::create(tmpAdu.rawData(), ModbusMessage::Rtu, QDateTime::currentDateTime(), false);
            emit modbusResponse(_queue.first().reply->requestGroupId(), msg);
        }

        return;
    }

    if (_queue.isEmpty())
        return;
    auto &current = _queue.first();

    // Special case for Diagnostics:ReturnQueryData. The response has no
    // length indicator and is just a simple echo of what we have send.
    if (tmpAdu.pdu().functionCode() == QModbusPdu::Diagnostics) {
        const QModbusResponse response = tmpAdu.pdu();
        if (canMatchRequestAndResponse(response, tmpAdu.serverAddress())) {
            quint16 subCode = 0xffff;
            response.decodeData(&subCode);
            if (subCode == Diagnostics::ReturnQueryData) {
                if (response.data() != current.requestPdu.data())
                    return; // echo does not match request yet
                aduSize = 2 + response.dataSize() + 2;
                if (tmpAdu.rawSize() < aduSize)
                    return; // echo matches, probably checksum missing
            }
        }
    }

    const QModbusSerialAdu adu(QModbusSerialAdu::Rtu, _responseBuffer.left(aduSize));
    _responseBuffer.remove(0, aduSize);

    qCDebug(QT_MODBUS) << "(RTU over TCP client) Received ADU:" << adu.rawData().toHex();
    if (QT_MODBUS().isDebugEnabled() && !_responseBuffer.isEmpty())
        qCDebug(QT_MODBUS_LOW) << "(RTU over TCP client) Pending buffer:" << _responseBuffer.toHex();

    // check CRC
    if (!adu.matchingChecksum()) {
        qCWarning(QT_MODBUS) << "(RTU over TCP client) Discarding response with wrong CRC, received:"
                             << adu.checksum<quint16>() << ", calculated CRC:"
                             << QModbusSerialAdu::calculateCRC(adu.data(), adu.size());
        if (!current.reply.isNull()) {
            current.reply->addIntermediateError(ModbusDevice::ResponseCrcError);

            const auto msg = ModbusMessage::create(adu.rawData(), ModbusMessage::Rtu, QDateTime::currentDateTime(), false);
            emit modbusResponse(current.reply->requestGroupId(), msg);
        }
        return;
    }

    const QModbusResponse response = adu.pdu();
    if (!canMatchRequestAndResponse(response, adu.serverAddress())) {
        qCWarning(QT_MODBUS) << "(RTU over TCP client) Cannot match response with open request, "
                                "ignoring";
        if (!current.reply.isNull())
            current.reply->addIntermediateError(ModbusDevice::ResponseRequestMismatch);
        return;
    }

    _state = ProcessReply;
    _responseTimer.stop();
    current.m_timerId = INT_MIN;

    processQueueElement(response, ModbusMessage::Rtu, adu.serverAddress(), _queue.dequeue());

    _state = Idle;
    scheduleNextRequest(_interFrameDelayMilliseconds);
}

///
/// \brief ModbusRtuTcpClient::on_bytesWritten
/// \param bytes
///
void ModbusRtuTcpClient::on_bytesWritten(qint64 bytes)
{
    if (_queue.isEmpty())
        return;
    auto &current = _queue.first();

    current.bytesWritten += bytes;
    if (current.bytesWritten != current.adu.size())
        return;

    qCDebug(QT_MODBUS) << "(RTU over TCP client) Send successful:" << current.requestPdu;

    if (!current.reply.isNull() && current.reply->type() == ModbusReply::Broadcast) {
        _state = ProcessReply;
        const int serverAddress = current.reply->serverAddress();
        processQueueElement({}, ModbusMessage::Rtu, serverAddress, _queue.dequeue());
        _state = Idle;
        scheduleNextRequest(_turnaroundDelay);
    } else {
        current.m_timerId = _responseTimer.start(timeout());
    }
}

///
/// \brief ModbusRtuTcpClient::on_responseTimeout
///
void ModbusRtuTcpClient::on_responseTimeout(int timerId)
{
    _responseTimer.stop();
    if (_state != State::WaitingForReplay || _queue.isEmpty())
        return;
    const auto &current = _queue.first();

    if (current.m_timerId != timerId)
        return;

    qCDebug(QT_MODBUS) << "(RTU over TCP client) Receive timeout:" << current.requestPdu;

    if (current.numberOfRetries <= 0) {
        auto item = _queue.dequeue();
        if (item.reply) {
            item.reply->setError(ModbusDevice::TimeoutError,
                                 QModbusClient::tr("Request timeout."));
        }
    }

    _state = Idle;
    scheduleNextRequest(_interFrameDelayMilliseconds);
}
