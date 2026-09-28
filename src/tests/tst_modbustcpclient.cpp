#include <QtTest>
#include <QTcpServer>
#include <QTcpSocket>
#include "modbustcpclient.h"

///
/// \brief Loopback Modbus TCP server that answers every request with a preset PDU.
///
class FakeModbusTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit FakeModbusTcpServer(QObject* parent = nullptr);

    bool listen();
    quint16 port() const;
    void setResponse(const QByteArray& pdu, int splitAt = 0);

private slots:
    void on_newConnection();
    void on_readyRead();
    void sendTail();

private:
    QTcpServer _server;
    QTcpSocket* _socket = nullptr;
    QByteArray _pdu;
    QByteArray _tail;
    int _splitAt = 0;
};

///
/// \brief FakeModbusTcpServer::FakeModbusTcpServer
/// \param parent
///
FakeModbusTcpServer::FakeModbusTcpServer(QObject* parent)
    : QObject(parent)
{
    connect(&_server, &QTcpServer::newConnection, this, &FakeModbusTcpServer::on_newConnection);
}

///
/// \brief Starts listening on a free loopback port.
///
bool FakeModbusTcpServer::listen()
{
    return _server.listen(QHostAddress::LocalHost);
}

///
/// \brief FakeModbusTcpServer::port
///
quint16 FakeModbusTcpServer::port() const
{
    return _server.serverPort();
}

///
/// \brief Sets the PDU sent back for every request.
/// \param pdu Function code followed by the response data.
/// \param splitAt When positive, the ADU is sent as two TCP segments split at this byte offset.
///
void FakeModbusTcpServer::setResponse(const QByteArray& pdu, int splitAt)
{
    _pdu = pdu;
    _splitAt = splitAt;
}

///
/// \brief FakeModbusTcpServer::on_newConnection
///
void FakeModbusTcpServer::on_newConnection()
{
    _socket = _server.nextPendingConnection();
    connect(_socket, &QTcpSocket::readyRead, this, &FakeModbusTcpServer::on_readyRead);
}

///
/// \brief Replies to a request, echoing its transaction and unit identifiers.
///
void FakeModbusTcpServer::on_readyRead()
{
    const QByteArray request = _socket->readAll();
    if (request.size() < 7)
        return;

    const quint16 length = quint16(_pdu.size() + 1);
    QByteArray adu = request.left(2);
    adu.append('\0').append('\0');
    adu.append(char(length >> 8)).append(char(length & 0xff));
    adu.append(request.at(6));
    adu.append(_pdu);

    if (_splitAt <= 0) {
        _socket->write(adu);
        return;
    }

    _socket->write(adu.left(_splitAt));
    _socket->flush();
    _tail = adu.mid(_splitAt);
    QTimer::singleShot(50, this, &FakeModbusTcpServer::sendTail);
}

///
/// \brief Sends the delayed second segment of a split ADU.
///
void FakeModbusTcpServer::sendTail()
{
    _socket->write(_tail);
}

///
/// \brief The TestModbusTcpClient class
///
class TestModbusTcpClient : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void rawResponse_data();
    void rawResponse();
    void readHoldingRegisters();

private:
    ModbusReply* waitForReply(ModbusReply* reply);

private:
    FakeModbusTcpServer* _server = nullptr;
    ModbusTcpClient* _client = nullptr;
};

///
/// \brief Starts the fake server and connects a client to it.
///
void TestModbusTcpClient::init()
{
    _server = new FakeModbusTcpServer;
    QVERIFY(_server->listen());

    _client = new ModbusTcpClient;
    _client->setConnectionParameter(ModbusDevice::NetworkAddressParameter, QStringLiteral("127.0.0.1"));
    _client->setConnectionParameter(ModbusDevice::NetworkPortParameter, _server->port());
    QVERIFY(_client->connectDevice());
    QTRY_COMPARE(_client->state(), ModbusDevice::ConnectedState);
}

///
/// \brief TestModbusTcpClient::cleanup
///
void TestModbusTcpClient::cleanup()
{
    delete _client;
    _client = nullptr;
    delete _server;
    _server = nullptr;
}

///
/// \brief Waits until the reply is finished.
/// \return The same reply, or nullptr on timeout.
///
ModbusReply* TestModbusTcpClient::waitForReply(ModbusReply* reply)
{
    if (!reply)
        return nullptr;

    QSignalSpy finishedSpy(reply, &ModbusReply::finished);
    return finishedSpy.wait(2000) ? reply : nullptr;
}

///
/// \brief TestModbusTcpClient::rawResponse_data
///
void TestModbusTcpClient::rawResponse_data()
{
    QTest::addColumn<QByteArray>("responsePdu");
    QTest::addColumn<int>("splitAt");
    QTest::addColumn<bool>("isException");

    QTest::newRow("custom function without data (#86)") << QByteArray::fromHex("64") << 0 << false;
    QTest::newRow("custom function with data") << QByteArray::fromHex("64010203") << 0 << false;
    QTest::newRow("custom function exception") << QByteArray::fromHex("e401") << 0 << true;
    QTest::newRow("custom function split across segments") << QByteArray::fromHex("64010203") << 9 << false;
    QTest::newRow("split inside MBAP header") << QByteArray::fromHex("64010203") << 4 << false;
}

///
/// \brief Raw responses must keep the function code and data exactly as received.
///
void TestModbusTcpClient::rawResponse()
{
    QFETCH(QByteArray, responsePdu);
    QFETCH(int, splitAt);
    QFETCH(bool, isException);

    _server->setResponse(responsePdu, splitAt);

    const QModbusRequest request(QModbusPdu::FunctionCode(0x64), QByteArray::fromHex("00"));
    const auto reply = waitForReply(_client->sendRawRequest(request, 1));
    QVERIFY(reply);

    const QModbusResponse response = reply->rawResult();
    QCOMPARE(quint8(response.functionCode()), quint8(0x64));
    QCOMPARE(response.isException(), isException);
    QCOMPARE(response.data(), responsePdu.mid(1));
}

///
/// \brief Standard responses are still decoded into register values.
///
void TestModbusTcpClient::readHoldingRegisters()
{
    _server->setResponse(QByteArray::fromHex("0304000a000b"));

    const QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, 0, 2);
    const auto reply = waitForReply(_client->sendReadRequest(unit, 1));
    QVERIFY(reply);

    QCOMPARE(reply->error(), ModbusDevice::NoError);
    QCOMPARE(reply->result().values(), QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, {10, 11}).values());
}

QTEST_GUILESS_MAIN(TestModbusTcpClient)
#include "tst_modbustcpclient.moc"
