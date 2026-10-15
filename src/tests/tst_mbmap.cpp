#include <QtTest>
#include "mbmap.h"

///
/// \brief Tests of reading .mbmap.json device maps and laying them out in windows
///
class TestMbMap : public QObject
{
    Q_OBJECT

private slots:
    void refusesOtherFiles();
    void readsTheMap();
    void groupsRegistersInWindows();
    void splitsFarAndMisalignedRegisters();
    void describesBitsAndScale();
};

namespace {

MbMap parse(const QByteArray& json)
{
    QString error;
    const auto map = MbMap::parse(json, &error);
    if(!error.isEmpty()) qWarning() << error;
    return map;
}

}

///
/// \brief Files that are not version 2 maps are refused with a reason.
///
void TestMbMap::refusesOtherFiles()
{
    QString error;
    MbMap::parse("not json", &error);
    QVERIFY(!error.isEmpty());

    MbMap::parse(R"({"mbmap": 3, "vendor": "A", "model": "B", "registers": []})", &error);
    QVERIFY(error.contains("3"));

    MbMap::parse(R"({"mbmap": 2, "vendor": "A", "registers": []})", &error);
    QVERIFY(!error.isEmpty());

    QVERIFY(MbMap::isMapFile("dir/em-100.mbmap.json"));
    QVERIFY(MbMap::isMapFile("EM.MBMAP.JSON"));
    QVERIFY(!MbMap::isMapFile("form.xml"));
}

///
/// \brief Connection defaults, address base, hex addresses and byte order are read;
/// registers with errors are left out.
///
void TestMbMap::readsTheMap()
{
    const auto map = parse("\xEF\xBB\xBF" R"({
        "mbmap": 2, "vendor": "Example Corp", "model": "EM-100",
        "unit_id": "0x10", "address_base": 1, "word_order": "little",
        "registers": [
            {"name": "Voltage", "area": "input", "address": "0x0001", "type": "float32", "unit": "V"},
            {"name": "Status", "area": "holding", "address": 10, "type": "uint16", "byte_order": "little"},
            {"name": "Bad area", "area": "Holding", "address": 1, "type": "uint16"},
            {"name": "Below base", "area": "holding", "address": 0, "type": "uint16"},
            {"name": "Coil as int", "area": "coil", "address": 1, "type": "uint16"},
            {"name": "Bool without bit", "area": "holding", "address": 1, "type": "bool"}
        ]
    })");

    QCOMPARE(map.title(), QString("Example Corp EM-100"));
    QCOMPARE(map.UnitId, quint8(16));
    QCOMPARE(map.AddressBase, 1);
    QCOMPARE(map.Registers.size(), 2);
    QCOMPARE(map.Registers[0].Address, quint16(0));
    QVERIFY(map.Registers[0].WordSwapped);
    QCOMPARE(map.Registers[1].Address, quint16(9));
    QVERIFY(map.Registers[1].ByteSwapped);
}

///
/// \brief One window per area, display mode and byte order.
///
void TestMbMap::groupsRegistersInWindows()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M",
        "registers": [
            {"name": "Voltage", "group": "Measurements", "area": "input", "address": 0, "type": "float32"},
            {"name": "Current", "group": "Measurements", "area": "input", "address": 6, "type": "float32"},
            {"name": "Energy", "area": "holding", "address": 256, "type": "uint32", "word_order": "little"},
            {"name": "Setpoint", "area": "holding", "address": 300, "type": "int16"},
            {"name": "Reset", "area": "coil", "address": 0, "type": "bool"},
            {"name": "Serial", "area": "holding", "address": 1024, "type": "string", "length": 8}
        ]
    })");

    const auto views = map.views();
    QCOMPARE(views.size(), 5);

    QCOMPARE(views[0].Area, QModbusDataUnit::Coils);
    QCOMPARE(views[0].Mode, DataDisplayMode::Binary);

    const auto& input = views[1];
    QCOMPARE(input.Area, QModbusDataUnit::InputRegisters);
    QCOMPARE(input.Mode, DataDisplayMode::FloatingPt);
    QCOMPARE(input.Address, quint16(0));
    QCOMPARE(input.Length, quint16(8));
    QCOMPARE(input.Descriptions.keys(), QList<quint16>({ 0, 6 }));
    QVERIFY(input.Title.contains("Measurements"));

    QCOMPARE(views[2].Mode, DataDisplayMode::SwappedUInt32);
    QCOMPARE(views[2].Length, quint16(2));
    QCOMPARE(views[3].Mode, DataDisplayMode::Int16);
    QCOMPARE(views[4].Mode, DataDisplayMode::Ansi);
    QCOMPARE(views[4].Length, quint16(8));
}

///
/// \brief Far apart registers and values that do not line up with the window's
/// 2-register values go to windows of their own.
///
void TestMbMap::splitsFarAndMisalignedRegisters()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M",
        "registers": [
            {"name": "A", "area": "holding", "address": 0, "type": "float32"},
            {"name": "B", "area": "holding", "address": 3, "type": "float32"},
            {"name": "C", "area": "holding", "address": 100, "type": "float32"},
            {"name": "D", "area": "holding", "address": 102, "type": "float32"}
        ]
    })");

    const auto views = map.views();
    QCOMPARE(views.size(), 3);
    QCOMPARE(views[0].Address, quint16(0));
    QCOMPARE(views[0].Length, quint16(2));
    QCOMPARE(views[1].Address, quint16(3));
    QCOMPARE(views[2].Address, quint16(100));
    QCOMPARE(views[2].Length, quint16(4));
}

///
/// \brief Descriptions name the bits, the unit and the scale, and share an address.
///
void TestMbMap::describesBitsAndScale()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M",
        "registers": [
            {"name": "Relay", "area": "holding", "address": 512, "type": "bool", "bit": 3},
            {"name": "Alarm", "area": "holding", "address": 512, "type": "bool", "bit": 4},
            {"name": "Code", "area": "holding", "address": 513, "type": "uint16", "bit": 8, "bits": 4},
            {"name": "Frequency", "area": "input", "address": 70, "type": "uint16", "scale": 0.01, "unit": "Hz"}
        ]
    })");

    const auto views = map.views();
    QCOMPARE(views.size(), 2);
    QCOMPARE(views[0].Mode, DataDisplayMode::UInt16);
    QCOMPARE(views[0].Descriptions.value(70), QString("Frequency [Hz] x0.01"));
    QCOMPARE(views[1].Mode, DataDisplayMode::Binary);
    QCOMPARE(views[1].Descriptions.value(512), QString("bit 3: Relay; bit 4: Alarm"));
    QCOMPARE(views[1].Descriptions.value(513), QString("bits 8-11: Code"));
}

QTEST_GUILESS_MAIN(TestMbMap)
#include "tst_mbmap.moc"
