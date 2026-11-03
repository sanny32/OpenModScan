#include <QtTest>
#include <limits>
#include "modbusclient.h" // RegisterType names used in saved window definitions.
#include "displaydefinition.h"
#include "modbusdataunit.h"
#include "numericutils.h"
#include "qmodbusadurtu.h"
#include "qmodbusadutcp.h"

#ifdef Q_OS_MACOS
#include "serialportutils.h"
#include <util.h>
#include <unistd.h>
#endif

class TestCore : public QObject
{
    Q_OBJECT
private slots:
    void numericValues_data();
    void numericValues();
    void addressLimits_data();
    void addressLimits();
    void dataUnit();
    void frames();
    void displaySettings_data();
    void displaySettings();
    void normalizesInvalidSettings();
#ifdef Q_OS_MACOS
    void serialPseudoTerminal();
    void serialBluetoothPorts();
    void serialHardwarePorts();
    void serialUnknownPort();
#endif
};

#ifdef Q_OS_MACOS
void TestCore::serialPseudoTerminal()
{
    int master = -1;
    int slave = -1;
    char name[1024] = {};
    QVERIFY(openpty(&master, &slave, name, nullptr, nullptr) == 0);
    QFile masterFile;
    QFile slaveFile;
    const bool masterOpened = masterFile.open(master, QIODevice::ReadWrite, QFileDevice::AutoCloseHandle);
    const bool slaveOpened = slaveFile.open(slave, QIODevice::ReadWrite, QFileDevice::AutoCloseHandle);
    if (!masterOpened)
        close(master);
    if (!slaveOpened)
        close(slave);
    QVERIFY(masterOpened && slaveOpened);

    const QString path = QString::fromLocal8Bit(name);
    QVERIFY(isVirtualPort(path));
    QVERIFY(isVirtualPort(QFileInfo(path).fileName()));
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QString alias = directory.filePath("serial-port");
    QVERIFY(QFile::link(path, alias));
    QVERIFY(isVirtualPort(alias));
}

void TestCore::serialBluetoothPorts()
{
    bool found = false;
    const QStringList filtered = getAvailableSerialPorts(true);
    for (const auto& port : QSerialPortInfo::availablePorts()) {
        if (!port.portName().contains("Bluetooth-Incoming-Port", Qt::CaseInsensitive))
            continue;
        found = true;
        QVERIFY(isVirtualPort(port.portName()));
        QVERIFY(isVirtualPort(port.systemLocation()));
        QString dialin = port.systemLocation();
        dialin.replace("/dev/cu.", "/dev/tty.");
        QVERIFY(isVirtualPort(dialin));
        QVERIFY(!filtered.contains(port.portName()));
    }
    if (!found)
        QSKIP("No Bluetooth serial port is currently published by macOS");
}

void TestCore::serialHardwarePorts()
{
    bool found = false;
    const QStringList filtered = getAvailableSerialPorts(true);
    for (const auto& port : QSerialPortInfo::availablePorts()) {
        // USB serial adapters and the built-in Apple Silicon UARTs.
        if (!port.hasVendorIdentifier() && port.portName() != "cu.debug-console" &&
            port.portName() != "cu.wlan-debug" && port.portName() != "cu.bluetooth-debug")
            continue;
        found = true;
        QVERIFY2(!isVirtualPort(port.portName()), qPrintable(port.portName()));
        QVERIFY(!isVirtualPort(port.systemLocation()));
        QVERIFY(filtered.contains(port.portName()));
    }
    if (!found)
        QSKIP("No hardware serial ports are connected");
}

void TestCore::serialUnknownPort()
{
    QVERIFY(!isVirtualPort(QString()));
    QVERIFY(!isVirtualPort("cu.openmodscan-nonexistent-port"));
}
#endif

void TestCore::numericValues_data()
{
    QTest::addColumn<DataDisplayMode>("mode");
    QTest::addColumn<QVector<quint16>>("registers");
    QTest::addColumn<QVariant>("expected");
    QTest::newRow("uint16-max") << DataDisplayMode::UInt16 << QVector<quint16>{0xffff} << QVariant(65535u);
    QTest::newRow("int16-negative") << DataDisplayMode::Int16 << QVector<quint16>{0xffff} << QVariant(-1);
    QTest::newRow("int32-min") << DataDisplayMode::Int32 << QVector<quint16>{0x8000, 0} << QVariant(std::numeric_limits<qint32>::min());
    QTest::newRow("uint32") << DataDisplayMode::UInt32 << QVector<quint16>{0x89ab, 0xcdef} << QVariant(0x89abcdefu);
    QTest::newRow("uint32-swapped") << DataDisplayMode::SwappedUInt32 << QVector<quint16>{0xcdef, 0x89ab} << QVariant(0x89abcdefu);
    QTest::newRow("float") << DataDisplayMode::FloatingPt << QVector<quint16>{0x3fc0, 0} << QVariant(1.5f);
    QTest::newRow("float-swapped") << DataDisplayMode::SwappedFP << QVector<quint16>{0, 0x3fc0} << QVariant(1.5f);
    QTest::newRow("double") << DataDisplayMode::DblFloat << QVector<quint16>{0xbff8, 0, 0, 0} << QVariant(-1.5);
    QTest::newRow("double-swapped") << DataDisplayMode::SwappedDbl << QVector<quint16>{0, 0, 0, 0xbff8} << QVariant(-1.5);
    QTest::newRow("int64-min") << DataDisplayMode::Int64 << QVector<quint16>{0x8000, 0, 0, 0} << QVariant::fromValue(std::numeric_limits<qint64>::min());
    QTest::newRow("uint64-max") << DataDisplayMode::UInt64 << QVector<quint16>{0xffff, 0xffff, 0xffff, 0xffff} << QVariant::fromValue(std::numeric_limits<quint64>::max());
    QTest::newRow("uint64-swapped") << DataDisplayMode::SwappedUInt64 << QVector<quint16>{0xcdef, 0x89ab, 0x4567, 0x0123} << QVariant::fromValue(quint64(0x0123456789abcdefULL));
}

void TestCore::numericValues()
{
    QFETCH(DataDisplayMode, mode);
    QFETCH(QVector<quint16>, registers);
    QFETCH(QVariant, expected);
    QCOMPARE(makeValue(registers, mode, ByteOrder::Direct), expected);
    QVERIFY(!makeValue(registers.mid(1), mode, ByteOrder::Direct).isValid());
    // Independently swap the bytes in each wire register, keeping word order.
    if (registers.size() > 1) {
        for (auto& reg : registers)
            reg = quint16((reg << 8) | (reg >> 8));
        QCOMPARE(makeValue(registers, mode, ByteOrder::Swapped), expected);
    }
}

void TestCore::addressLimits_data()
{
    QTest::addColumn<AddressSpace>("space");
    QTest::addColumn<bool>("zeroBased");
    for (const auto space : {AddressSpace::Addr5Digits, AddressSpace::Addr6Digits})
        for (const bool zero : {false, true})
            QTest::newRow(qPrintable(QString("%1-base%2").arg(int(space)).arg(zero ? 0 : 1))) << space << zero;
}

void TestCore::addressLimits()
{
    QFETCH(AddressSpace, space);
    QFETCH(bool, zeroBased);
    const int size = space == AddressSpace::Addr5Digits ? 9999 : 65536;
    const int first = zeroBased ? 0 : 1;
    const int last = first + size - 1;
    QCOMPARE(ModbusLimits::addressRange(space, zeroBased).from(), first);
    QCOMPARE(ModbusLimits::addressRange(space, zeroBased).to(), last);
    QVERIFY(ModbusLimits::isReadable(first, zeroBased, 125, space));
    QVERIFY(ModbusLimits::isReadable(last, zeroBased, 1, space));
    QVERIFY(!ModbusLimits::isReadable(first - 1, zeroBased, 1, space));
    QVERIFY(!ModbusLimits::isReadable(last + 1, zeroBased, 1, space));
    QVERIFY(!ModbusLimits::isReadable(last, zeroBased, 2, space));
    QVERIFY(!ModbusLimits::isReadable(first, zeroBased, 0, space));
    QCOMPARE(ModbusLimits::lengthRange(last, zeroBased, space).to(), 1);
    QCOMPARE(ModbusLimits::lengthRange(first, zeroBased, space).to(), 125);
}

void TestCore::dataUnit()
{
    ModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, 100, 3);
    for (int i = -1; i <= 3; ++i)
        QVERIFY(!unit.hasValue(i));
    unit.setValue(1, 0); // A received zero must differ from an unread register.
    QVERIFY(unit.hasValue(1));
    QCOMPARE(unit.value(1), quint16(0));
    unit.setValue(2, 0xffff);
    QCOMPARE(unit.value(2), quint16(0xffff));
    unit.setValue(-1, 10);
    unit.setValue(3, 10);
    QVERIFY(!unit.hasValue(0));
    QVERIFY(!unit.hasValue(-1));
    QVERIFY(!unit.hasValue(3));
}

void TestCore::frames()
{
    // Fixed wire fixtures: CRC and MBAP expectations do not use production encoders.
    const auto rtuBytes = QByteArray::fromHex("01030000000ac5cd");
    QModbusAduRtu rtu(rtuBytes);
    QVERIFY(rtu.isValid());
    QCOMPARE(rtu.serverAddress(), quint8(1));
    QCOMPARE(rtu.pdu().data(), QByteArray::fromHex("0000000a"));
    auto corrupt = rtuBytes;
    corrupt[3] = '\x01';
    QVERIFY(!QModbusAduRtu(corrupt).isValid());
    const auto tcpBytes = QByteArray::fromHex("12340000000611030000000a");
    QModbusAduTcp tcp(tcpBytes);
    QVERIFY(tcp.isValid());
    QCOMPARE(tcp.transactionId(), quint16(0x1234));
    QCOMPARE(tcp.serverAddress(), quint8(17));
    tcp.setTransactionId(0xabcd);
    QCOMPARE(tcp.transactionId(), quint16(0xabcd));
    auto badLength = tcpBytes;
    badLength[5] = '\x07';
    QVERIFY(!QModbusAduTcp(badLength).isValid());
    for (int i = 0; i < rtuBytes.size(); ++i)
        QVERIFY(!QModbusAduRtu(rtuBytes.left(i)).isValid());
    for (int i = 0; i < tcpBytes.size(); ++i)
        QVERIFY(!QModbusAduTcp(tcpBytes.left(i)).isValid());
    rtu.setRawData({});
    tcp.setRawData({});
    QVERIFY(!rtu.isValid());
    QVERIFY(!tcp.isValid());
}

void TestCore::displaySettings_data()
{
    QTest::addColumn<bool>("xml");
    QTest::newRow("ini") << false;
    QTest::newRow("xml") << true;
}

void TestCore::displaySettings()
{
    QFETCH(bool, xml);
    DisplayDefinition original;
    original.FormName = QString::fromUtf8("Насос <A&B>");
    original.ScanRate = 250;
    original.DeviceId = 17;
    original.PointType = QModbusDataUnit::InputRegisters;
    original.PointAddress = 420;
    original.Length = 12;
    original.ZeroBasedAddress = true;
    original.AddrSpace = AddressSpace::Addr5Digits;
    original.HexAddress = original.AutoscrollLog = true;
    original.HexViewAddress = original.HexViewDeviceId = original.HexViewLength = true;
    original.LeadingZeros = false;
    original.LogViewLimit = 75;
    original.DataViewColumnsDistance = 8;
    DisplayDefinition restored;
    if (xml) {
        QString data;
        QXmlStreamWriter writer(&data);
        writer << original;
        QXmlStreamReader reader(data);
        QVERIFY(reader.readNextStartElement());
        reader >> restored;
        QVERIFY(!reader.hasError());
    } else {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        {
            QSettings settings(dir.filePath("test.ini"), QSettings::IniFormat);
            settings << original;
            settings.sync();
            QCOMPARE(settings.status(), QSettings::NoError);
        }
        QSettings settings(dir.filePath("test.ini"), QSettings::IniFormat);
        settings >> restored;
    }
    // Compare every serialized field, including flags and escaped Unicode text.
    QString expected, actual;
    QXmlStreamWriter expectedWriter(&expected), actualWriter(&actual);
    expectedWriter << original;
    actualWriter << restored;
    QCOMPARE(actual, expected);
}

void TestCore::normalizesInvalidSettings()
{
    DisplayDefinition dd;
    dd.ScanRate = 0;
    dd.PointAddress = 65535;
    dd.Length = 0;
    dd.LogViewLimit = 0;
    dd.DataViewColumnsDistance = 100;
    dd.ZeroBasedAddress = true;
    dd.ShowHexViewButtons = false;
    dd.HexViewAddress = dd.HexViewDeviceId = dd.HexViewLength = true;
    dd.normalize();
    QCOMPARE(dd.ScanRate, quint32(20));
    QCOMPARE(dd.Length, quint16(1));
    QCOMPARE(dd.LogViewLimit, quint16(4));
    QCOMPARE(dd.DataViewColumnsDistance, quint16(32));
    QVERIFY(!dd.HexViewAddress && !dd.HexViewDeviceId && !dd.HexViewLength);
    QVERIFY(ModbusLimits::isReadable(dd.PointAddress, dd.ZeroBasedAddress, dd.Length, dd.AddrSpace));
}

QTEST_GUILESS_MAIN(TestCore)
#include "tst_core.moc"
