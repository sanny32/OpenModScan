#include <QtTest>
#include <QTcpServer>
#include <QTcpSocket>
#include "modbustcpclient.h"
#include "modbusclient.h"
#include "modbustcpscanner.h"

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
    QByteArray lastRequest;
    int requestCount = 0;

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
    QByteArray _incoming;
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
    _incoming.clear();
    _socket = _server.nextPendingConnection();
    connect(_socket, &QTcpSocket::readyRead, this, &FakeModbusTcpServer::on_readyRead);
}

///
/// \brief Replies to a request, echoing its transaction and unit identifiers.
///
void FakeModbusTcpServer::on_readyRead()
{
    _incoming += _socket->readAll();
    if (_incoming.size() < 7)
        return;
    const int requestSize = 6 + (quint8(_incoming[4]) << 8) + quint8(_incoming[5]);
    if (_incoming.size() < requestSize)
        return;
    const QByteArray request = _incoming.left(requestSize);
    _incoming.remove(0, requestSize);
    lastRequest = request;
    ++requestCount;
    if (_pdu.isEmpty())
        return; // A silent server is used by timeout tests.

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
    void standardRequest_data();
    void standardRequest();
    void timeout();
    void timeout_data();
    void protocolException();
    void maskWrite();
    void scanner_data();
    void scanner();
    void cancelScanner();
    void disconnectedRequest();

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


void TestModbusTcpClient::standardRequest_data()
{
    QTest::addColumn<int>("area");
    QTest::addColumn<bool>("write");
    QTest::addColumn<QVector<quint16>>("values");
    QTest::addColumn<QByteArray>("requestPdu");
    QTest::addColumn<QByteArray>("responsePdu");
    const QVector<quint16> bits{1, 0, 1, 0, 0, 1, 0, 1, 1};
    QTest::newRow("read-coils") << int(QModbusDataUnit::Coils) << false << bits
        << QByteArray::fromHex("0101230009") << QByteArray::fromHex("0102a501");
    QTest::newRow("read-discrete-inputs") << int(QModbusDataUnit::DiscreteInputs) << false << bits
        << QByteArray::fromHex("0201230009") << QByteArray::fromHex("0202a501");
    for (const auto area : {QModbusDataUnit::HoldingRegisters, QModbusDataUnit::InputRegisters}) {
        const QByteArray fc(1, area == QModbusDataUnit::HoldingRegisters ? '\x03' : '\x04');
        QTest::newRow(area == QModbusDataUnit::HoldingRegisters ? "read-holding" : "read-input")
            << int(area) << false << QVector<quint16>{0x1234, 0xffff}
            << fc + QByteArray::fromHex("01230002") << fc + QByteArray::fromHex("041234ffff");
    }
    QTest::newRow("write-coil-on") << int(QModbusDataUnit::Coils) << true << QVector<quint16>{1}
        << QByteArray::fromHex("050123ff00") << QByteArray::fromHex("050123ff00");
    QTest::newRow("write-coil-off") << int(QModbusDataUnit::Coils) << true << QVector<quint16>{0}
        << QByteArray::fromHex("0501230000") << QByteArray::fromHex("0501230000");
    QTest::newRow("write-register") << int(QModbusDataUnit::HoldingRegisters) << true << QVector<quint16>{0xabcd}
        << QByteArray::fromHex("060123abcd") << QByteArray::fromHex("060123abcd");
    QTest::newRow("write-multiple-coils") << int(QModbusDataUnit::Coils) << true << bits
        << QByteArray::fromHex("0f0123000902a501") << QByteArray::fromHex("0f01230009");
    QTest::newRow("write-multiple-registers") << int(QModbusDataUnit::HoldingRegisters) << true << QVector<quint16>{0x1234, 0xffff}
        << QByteArray::fromHex("1001230002041234ffff") << QByteArray::fromHex("1001230002");
}

void TestModbusTcpClient::standardRequest()
{
    QFETCH(int, area);
    QFETCH(bool, write);
    QFETCH(QVector<quint16>, values);
    QFETCH(QByteArray, requestPdu);
    QFETCH(QByteArray, responsePdu);
    _server->setResponse(responsePdu);
    const auto type = QModbusDataUnit::RegisterType(area);
    QModbusDataUnit unit(type, 0x123, quint16(values.size()));
    if (write)
        unit.setValues(values);
    const auto reply = waitForReply(write ? _client->sendWriteRequest(unit, 17, 42)
                                         : _client->sendReadRequest(unit, 17, 42));
    QVERIFY(reply);
    QCOMPARE(reply->error(), ModbusDevice::NoError);
    QCOMPARE(reply->serverAddress(), 17);
    QCOMPARE(reply->requestGroupId(), 42);
    QCOMPARE(_server->lastRequest.mid(7), requestPdu);
    QCOMPARE(quint8(_server->lastRequest.at(6)), quint8(17));
    QCOMPARE(reply->result().registerType(), type);
    QCOMPARE(reply->result().startAddress(), 0x123);
    if (!write && (type == QModbusDataUnit::Coils || type == QModbusDataUnit::DiscreteInputs)) {
        // The transport decoder returns whole bytes, including unused padding bits.
        QCOMPARE(reply->result().values().mid(0, values.size()), values);
        for (int i = values.size(); i < reply->result().values().size(); ++i)
            QCOMPARE(reply->result().value(i), quint16(0));
    } else if (write && type == QModbusDataUnit::Coils && values.size() == 1) {
        QCOMPARE(reply->result().value(0), quint16(values[0] ? 0xff00 : 0));
    } else if (!write || values.size() == 1) {
        QCOMPARE(reply->result().values(), values);
    }
}

void TestModbusTcpClient::timeout()
{
    QFETCH(int, retries);
    _client->setTimeout(50);
    _client->setNumberOfRetries(retries);
    auto reply = _client->sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 1), 1);
    QVERIFY(reply);
    QSignalSpy finished(reply, &ModbusReply::finished);
    QTRY_VERIFY_WITH_TIMEOUT(reply->isFinished(), 2000);
    QCOMPARE(reply->error(), ModbusDevice::TimeoutError);
    QCOMPARE(finished.count(), 1);
    QCOMPARE(_server->requestCount, retries + 1);
}

void TestModbusTcpClient::timeout_data()
{
    QTest::addColumn<int>("retries");
    QTest::newRow("no-retry") << 0;
    QTest::newRow("one-retry") << 1;
}

void TestModbusTcpClient::protocolException()
{
    _server->setResponse(QByteArray::fromHex("8302"));
    auto reply = waitForReply(_client->sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 1), 1));
    QVERIFY(reply);
    QCOMPARE(reply->error(), ModbusDevice::ProtocolError);
    QCOMPARE(reply->rawResult().exceptionCode(), QModbusPdu::IllegalDataAddress);
    QVERIFY(!reply->result().isValid());
}

void TestModbusTcpClient::maskWrite()
{
    _client->disconnectDevice();
    QTRY_COMPARE(_client->state(), ModbusDevice::UnconnectedState);
    ModbusClient client;
    ConnectionDetails connection;
    connection.Type = ConnectionType::Tcp;
    connection.ModbusParams.Mode = TransmissionMode::IP;
    connection.TcpParams.ServicePort = _server->port();
    client.connectDevice(connection);
    QTRY_COMPARE(client.state(), ModbusDevice::ConnectedState);
    const auto pdu = QByteArray::fromHex("160123ff000055");
    _server->setResponse(pdu);
    bool finished = false;
    connect(&client, &ModbusClient::modbusReply, this, [&](const ModbusReply* reply) {
        finished = true;
        QCOMPARE(reply->error(), ModbusDevice::NoError);
        QCOMPARE(reply->rawResult().data(), pdu.mid(1));
        QCOMPARE(reply->requestGroupId(), 42);
    });
    ModbusMaskWriteParams params{};
    params.DeviceId = 17;
    params.Address = 0x124; // One-based UI address becomes wire address 0x123.
    params.AndMask = 0xff00;
    params.OrMask = 0x55;
    client.maskWriteRegister(params, 42);
    QTRY_VERIFY(finished);
    QCOMPARE(_server->lastRequest.mid(7), pdu);
}

void TestModbusTcpClient::disconnectedRequest()
{
    _client->disconnectDevice();
    QTRY_COMPARE(_client->state(), ModbusDevice::UnconnectedState);
    QVERIFY(!_client->sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, 0, 1), 1));
    QCOMPARE(_client->error(), ModbusDevice::ConnectionError);
}

void TestModbusTcpClient::scanner_data()
{
    QTest::addColumn<QByteArray>("response");
    QTest::addColumn<int>("foundCount");
    QTest::newRow("responding-devices") << QByteArray::fromHex("0302002a") << 2;
    QTest::newRow("device-exception") << QByteArray::fromHex("8302") << 2;
    QTest::newRow("gateway-no-target") << QByteArray::fromHex("830b") << 0;
    QTest::newRow("silent-devices") << QByteArray() << 0;
}

void TestModbusTcpClient::scanner()
{
    QFETCH(QByteArray, response);
    QFETCH(int, foundCount);
    _server->setResponse(response);
    ScanParams params;
    // Allow for event-loop scheduling delays on shared CI runners. Timeout
    // behavior is covered separately by timeout(); this test checks discovery.
    params.Timeout = 250;
    params.DeviceIds = {17, 18};
    params.Request = QModbusRequest(QModbusPdu::ReadHoldingRegisters, quint16(0), quint16(1));
    ConnectionDetails connection;
    connection.Type = ConnectionType::Tcp;
    connection.TcpParams.ServicePort = _server->port();
    params.ConnParams.append(connection);
    ModbusTcpScanner scanner(params);
    QSignalSpy found(&scanner, &ModbusScanner::found);
    QSignalSpy finished(&scanner, &ModbusScanner::finished);
    QSignalSpy progress(&scanner, &ModbusScanner::progress);
    scanner.startScan();
    QVERIFY(scanner.inProgress());
    QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, 3000);
    QVERIFY(!scanner.inProgress());
    QCOMPARE(_server->requestCount, 2);
    QCOMPARE(found.count(), foundCount);
    for (int i = 0; i < found.count(); ++i) {
        QCOMPARE(found.at(i).at(1).toInt(), 17 + i);
        QCOMPARE(found.at(i).at(2).toBool(), false);
    }
    QVERIFY(!progress.isEmpty());
    QCOMPARE(progress.last().at(2).toInt(), 100);
}

void TestModbusTcpClient::cancelScanner()
{
    ScanParams params;
    ConnectionDetails connection;
    connection.Type = ConnectionType::Tcp;
    connection.TcpParams.ServicePort = _server->port();
    params.ConnParams.append(connection);
    ModbusTcpScanner scanner(params);
    QSignalSpy found(&scanner, &ModbusScanner::found);
    QSignalSpy finished(&scanner, &ModbusScanner::finished);
    scanner.startScan();
    scanner.stopScan();
    QTest::qWait(250); // Let the pending host-probe callback run after cancellation.
    QVERIFY(!scanner.inProgress());
    QCOMPARE(finished.count(), 1);
    QCOMPARE(found.count(), 0);
}

QTEST_GUILESS_MAIN(TestModbusTcpClient)
#include "tst_modbustcpclient.moc"
