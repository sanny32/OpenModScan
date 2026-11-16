#include <QtTest>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSerialPort>
#include "modbusrtuclient.h"
#include "modbusrtutcpclient.h"

class TestModbusRtuTraffic : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data();
    void init();
    void cleanup();
    void response_data();
    void response();

private:
    QTcpServer _server;
    ModbusClientPrivate* _client = nullptr;
    QIODevice* _peer = nullptr;
};

///
/// \brief Runs the traffic checks for both RTU transports when a serial port pair is available.
///
void TestModbusRtuTraffic::initTestCase_data()
{
    QTest::addColumn<bool>("serial");
    QTest::newRow("rtu-over-tcp") << false;
    QTest::newRow("serial-rtu") << true;
}

///
/// \brief Connects the client to a local peer without requiring a Modbus device.
///
void TestModbusRtuTraffic::init()
{
    QFETCH_GLOBAL(bool, serial);
    if (serial) {
        const QString clientPort = qEnvironmentVariable("OMODSCAN_TEST_SERIAL_CLIENT");
        const QString peerPort = qEnvironmentVariable("OMODSCAN_TEST_SERIAL_PEER");
        if (clientPort.isEmpty() || peerPort.isEmpty())
            QSKIP("A connected virtual serial port pair is not configured (OMODSCAN_TEST_SERIAL_CLIENT/PEER).");

        auto peer = new QSerialPort(this);
        _peer = peer;
        peer->setPortName(peerPort);
        QVERIFY(peer->setBaudRate(QSerialPort::Baud19200));
        QVERIFY(peer->setParity(QSerialPort::NoParity));
        QVERIFY(peer->open(QIODevice::ReadWrite));

        _client = new ModbusRtuClient;
        _client->setConnectionParameter(ModbusDevice::SerialPortNameParameter, clientPort);
        _client->setConnectionParameter(ModbusDevice::SerialParityParameter, QSerialPort::NoParity);
    } else {
        QVERIFY(_server.listen(QHostAddress::LocalHost));
        _client = new ModbusRtuTcpClient;
        _client->setConnectionParameter(ModbusDevice::NetworkAddressParameter, QStringLiteral("127.0.0.1"));
        _client->setConnectionParameter(ModbusDevice::NetworkPortParameter, _server.serverPort());
    }

    _client->setTimeout(1500);
    _client->setNumberOfRetries(0);
    QVERIFY(_client->connectDevice());
    QTRY_COMPARE(_client->state(), ModbusDevice::ConnectedState);
    if (!serial) {
        QTRY_VERIFY(_server.hasPendingConnections());
        _peer = _server.nextPendingConnection();
    }
}

///
/// \brief Releases both ends of the connection between test cases.
///
void TestModbusRtuTraffic::cleanup()
{
    delete _client;
    _client = nullptr;
    delete _peer;
    _peer = nullptr;
    _server.close();
}

///
/// \brief Covers complete, fragmented, corrupt and unfinished RTU responses.
///
void TestModbusRtuTraffic::response_data()
{
    QTest::addColumn<QByteArray>("pdu");
    QTest::addColumn<int>("splitAt");
    QTest::addColumn<bool>("badCrc");
    QTest::addColumn<bool>("incomplete");

    const QByteArray registers = QByteArray::fromHex("0304000a000b");
    QTest::newRow("complete") << registers << 0 << false << false;
    QTest::newRow("split-payload") << registers << 5 << false << false;
    QTest::newRow("split-before-crc-95") << registers << 7 << false << false;
    QTest::newRow("split-crc") << registers << 8 << false << false;
    QTest::newRow("byte-by-byte") << registers << 1 << false << false;
    QTest::newRow("exception") << QByteArray::fromHex("8302") << 0 << false << false;
    QTest::newRow("split-exception") << QByteArray::fromHex("8302") << 4 << false << false;
    QTest::newRow("bad-crc") << registers << 0 << true << false;
    QTest::newRow("split-bad-crc") << registers << 7 << true << false;
    QTest::newRow("missing-crc") << registers << 7 << false << true;
}

///
/// \brief Emits one traffic entry per complete frame and none for an unfinished response.
///
void TestModbusRtuTraffic::response()
{
    QFETCH(QByteArray, pdu);
    QFETCH(int, splitAt);
    QFETCH(bool, badCrc);
    QFETCH(bool, incomplete);

    const QModbusResponse response(QModbusPdu::FunctionCode(quint8(pdu.at(0))), pdu.mid(1));
    QByteArray adu = ModbusMessage::create(response, ModbusMessage::Rtu, 1, 0,
                                          QDateTime::currentDateTime(), false)->rawData();
    if (badCrc)
        adu[adu.size() - 1] = char(quint8(adu.at(adu.size() - 1)) ^ 0xff);

    QSignalSpy traffic(_client, &ModbusClientPrivate::modbusResponse);
    QVERIFY(traffic.isValid());
    QSignalSpy incoming(_client->device(), &QIODevice::readyRead);
    QVERIFY(incoming.isValid());
    const QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, 0, 2);
    auto reply = _client->sendReadRequest(unit, 1, 95);
    QVERIFY(reply);
    QTRY_COMPARE(_peer->bytesAvailable(), qint64(8));
    _peer->readAll();

    int sent = 0;
    while (sent < adu.size()) {
        const int count = splitAt == 1 ? 1 : (sent == 0 && splitAt > 0 ? splitAt : adu.size() - sent);
        QCOMPARE(_peer->write(adu.mid(sent, count)), qint64(count));
        sent += count;
        if (sent < adu.size()) {
            QTRY_VERIFY(!incoming.isEmpty());
            incoming.clear();
            QCOMPARE(traffic.count(), 0);
            QVERIFY(!reply->isFinished());
            if (incomplete)
                break;
        }
    }

    QTRY_VERIFY(reply->isFinished());
    QCOMPARE(traffic.count(), incomplete ? 0 : 1);
    if (!incomplete) {
        const auto entry = traffic.first();
        QCOMPARE(entry.at(0).toInt(), 95);
        const auto message = qvariant_cast<QSharedPointer<const ModbusMessage>>(entry.at(1));
        QVERIFY(message);
        QCOMPARE(message->rawData(), adu);
        QCOMPARE(message->isRequest(), false);
        QCOMPARE(message->isValid(), !badCrc);
    }

    if (incomplete || badCrc) {
        QCOMPARE(reply->error(), ModbusDevice::TimeoutError);
        QCOMPARE(reply->intermediateErrors().contains(ModbusDevice::ResponseCrcError), badCrc);
    } else if (response.isException()) {
        QCOMPARE(reply->error(), ModbusDevice::ProtocolError);
        QCOMPARE(reply->rawResult().exceptionCode(), response.exceptionCode());
    } else {
        QCOMPARE(reply->error(), ModbusDevice::NoError);
        QCOMPARE(reply->result().values(), QVector<quint16>({10, 11}));
    }
}

QTEST_GUILESS_MAIN(TestModbusRtuTraffic)
#include "tst_modbusrtutraffic.moc"
