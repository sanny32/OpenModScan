#include <QtTest>
#include <tuple>
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
    void respectsTheDeviceReadLimits();
    void keepsWriteOnlyRegistersOutOfReads();
    void ignoresByteOrderForBool();
    void placesTheLastAddressWithBase1();
    void pollsTheLastAddressWithBase1();
    void widensTheAddressSpaceForTheMap();
    void keepsWriteOnlyBarriersOfRegistersItCannotShow();
    void keepsDownloadedMapsInsideTheirFolder();
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

///
/// \brief max_read_registers and max_read_bits cap each window together with
/// Open ModScan's own limit; a register longer than the cap is left out.
///
void TestMbMap::respectsTheDeviceReadLimits()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M",
        "max_read_registers": 2, "max_read_bits": 3,
        "registers": [
            {"name": "A", "area": "holding", "address": 0, "type": "uint16"},
            {"name": "B", "area": "holding", "address": 1, "type": "uint16"},
            {"name": "C", "area": "holding", "address": 2, "type": "uint16"},
            {"name": "D", "area": "holding", "address": 3, "type": "uint16"},
            {"name": "Energy", "area": "input", "address": 0, "type": "float64"},
            {"name": "Q1", "area": "coil", "address": 0, "type": "bool"},
            {"name": "Q2", "area": "coil", "address": 1, "type": "bool"},
            {"name": "Q3", "area": "coil", "address": 2, "type": "bool"},
            {"name": "Q4", "area": "coil", "address": 3, "type": "bool"},
            {"name": "Q5", "area": "coil", "address": 4, "type": "bool"}
        ]
    })");
    QCOMPARE(map.MaxReadRegisters, 2);
    QCOMPARE(map.MaxReadBits, 3);

    QList<std::tuple<QModbusDataUnit::RegisterType, quint16, quint16>> blocks;
    for(auto&& v : map.views())
        blocks.append({ v.Area, v.Address, v.Length });

    // The float64 needs 4 registers in one read: it cannot be read at all
    QCOMPARE(blocks, (QList<std::tuple<QModbusDataUnit::RegisterType, quint16, quint16>>{
        { QModbusDataUnit::Coils, 0, 3 },
        { QModbusDataUnit::Coils, 3, 2 },
        { QModbusDataUnit::HoldingRegisters, 0, 2 },
        { QModbusDataUnit::HoldingRegisters, 2, 2 },
    }));

    // Out of range limits keep the defaults
    const auto loose = parse(R"({"mbmap": 2, "vendor": "V", "model": "M",
        "max_read_registers": 500, "max_read_bits": 0, "registers": []})");
    QCOMPARE(loose.MaxReadRegisters, 125);
    QCOMPARE(loose.MaxReadBits, 2000);
}

///
/// \brief A write-only register is not read, and no window reads across its
/// address; an address another register reads stays readable.
///
void TestMbMap::keepsWriteOnlyRegistersOutOfReads()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M",
        "registers": [
            {"name": "R0", "area": "holding", "address": 0, "type": "uint16"},
            {"name": "Command", "area": "holding", "address": 1, "type": "uint16", "access": "W"},
            {"name": "R2", "area": "holding", "address": 2, "type": "uint16", "access": "RW"},
            {"name": "Run", "area": "holding", "address": 10, "type": "bool", "bit": 0, "access": "W"},
            {"name": "Running", "area": "holding", "address": 10, "type": "bool", "bit": 1},
            {"name": "Ready", "area": "holding", "address": 11, "type": "bool", "bit": 0}
        ]
    })");
    QVERIFY(!map.Registers[1].Readable);
    QVERIFY(map.Registers[2].Readable);

    const auto views = map.views();
    QCOMPARE(views.size(), 3);
    QCOMPARE(views[0].Address, quint16(0));
    QCOMPARE(views[0].Length, quint16(1));
    QCOMPARE(views[1].Address, quint16(2));
    QCOMPARE(views[1].Length, quint16(1));
    for(auto&& v : views)
        for(auto&& text : v.Descriptions)
            QVERIFY(!text.contains("Command") && !text.contains("Run;") && text != "bit 0: Run");

    // Address 10 is also read for "Running", so its window may cover it
    QCOMPARE(views[2].Address, quint16(10));
    QCOMPARE(views[2].Length, quint16(2));
}

///
/// \brief Byte order does not apply to bool: it keeps the direct order, so
/// the bit the description names is the bit shown.
///
void TestMbMap::ignoresByteOrderForBool()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M", "byte_order": "little",
        "registers": [
            {"name": "Relay", "area": "holding", "address": 0, "type": "bool", "bit": 0},
            {"name": "Count", "area": "holding", "address": 1, "type": "uint16"}
        ]
    })");
    const auto views = map.views();
    QCOMPARE(views.size(), 2);
    for(auto&& v : views) {
        if(v.Mode == DataDisplayMode::Binary)
            QCOMPARE(v.Order, ByteOrder::Direct);
        else
            QCOMPARE(v.Order, ByteOrder::Swapped); // the other types still follow it
    }
}

///
/// \brief With address_base 1, the last wire address (65535) is address 65536,
/// which a 16-bit start address cannot hold: the window counts from 0 there.
///
void TestMbMap::placesTheLastAddressWithBase1()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M", "address_base": 1,
        "registers": [
            {"name": "First", "area": "holding", "address": 1, "type": "uint16"},
            {"name": "Last", "area": "holding", "address": 65536, "type": "uint16"}
        ]
    })");
    const auto views = map.views();
    QCOMPARE(views.size(), 2);

    const auto first = MbMap::placement(views[0], map.AddressBase);
    QVERIFY(!first.ZeroBased);
    QCOMPARE(first.Address, quint16(1));

    QCOMPARE(views[1].Address, quint16(65535));
    const auto last = MbMap::placement(views[1], map.AddressBase);
    QVERIFY(last.ZeroBased);
    QCOMPARE(last.Address, quint16(65535));

    MbMap::View zero;
    zero.Address = 7;
    const auto p = MbMap::placement(zero, 0);
    QVERIFY(p.ZeroBased);
    QCOMPARE(p.Address, quint16(7));
}

///
/// \brief A window placed at the last wire address passes the check the
/// window makes before polling.
///
void TestMbMap::pollsTheLastAddressWithBase1()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M", "address_base": 1,
        "registers": [
            {"name": "Last", "area": "holding", "address": 65536, "type": "uint16"}
        ]
    })");
    const auto views = map.views();
    QCOMPARE(views.size(), 1);

    DisplayDefinition dd;
    MbMap::setUpWindow(views[0], map.AddressBase, dd);
    dd.normalize();

    QCOMPARE(dd.PointAddress, quint16(65535));
    QCOMPARE(dd.Length, quint16(1));
    QVERIFY(ModbusLimits::isReadable(dd.PointAddress, dd.ZeroBasedAddress, dd.Length, dd.AddrSpace));

    // the same boundary in both numberings, and one past it
    QVERIFY(ModbusLimits::isReadable(65536, false, 1, AddressSpace::Addr6Digits));
    QVERIFY(!ModbusLimits::isReadable(65535, true, 2, AddressSpace::Addr6Digits));
    QVERIFY(!ModbusLimits::isReadable(65536, false, 2, AddressSpace::Addr6Digits));
    QVERIFY(ModbusLimits::isReadable(9998, true, 1, AddressSpace::Addr5Digits));
    QVERIFY(!ModbusLimits::isReadable(9999, true, 1, AddressSpace::Addr5Digits));
}

///
/// \brief A window whose registers lie past a 5-digit address space opens
/// in a 6-digit one instead of being moved to another address; the others
/// keep the address space the window had.
///
void TestMbMap::widensTheAddressSpaceForTheMap()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M", "address_base": 1,
        "registers": [
            {"name": "Low", "area": "holding", "address": 100, "type": "uint16"},
            {"name": "High", "area": "holding", "address": 40001, "type": "uint16"}
        ]
    })");
    const auto views = map.views();
    QCOMPARE(views.size(), 2);

    DisplayDefinition low;
    low.AddrSpace = AddressSpace::Addr5Digits;
    MbMap::setUpWindow(views[0], map.AddressBase, low);
    low.normalize();
    QCOMPARE(low.AddrSpace, AddressSpace::Addr5Digits);
    QCOMPARE(low.PointAddress, quint16(100));

    DisplayDefinition high;
    high.AddrSpace = AddressSpace::Addr5Digits;
    MbMap::setUpWindow(views[1], map.AddressBase, high);
    high.normalize();
    QCOMPARE(high.AddrSpace, AddressSpace::Addr6Digits);
    QCOMPARE(high.PointAddress, quint16(40001));
    QVERIFY(ModbusLimits::isReadable(high.PointAddress, high.ZeroBasedAddress, high.Length, high.AddrSpace));
}

///
/// \brief A write-only register that is not shown (here a string longer than
/// a read) still keeps the windows from reading across its addresses.
///
void TestMbMap::keepsWriteOnlyBarriersOfRegistersItCannotShow()
{
    const auto map = parse(R"({
        "mbmap": 2, "vendor": "V", "model": "M",
        "registers": [
            {"name": "R0", "area": "holding", "address": 0, "type": "uint16"},
            {"name": "Blob", "area": "holding", "address": 1, "type": "string", "length": 126, "access": "W"},
            {"name": "R2", "area": "holding", "address": 2, "type": "uint16"}
        ]
    })");
    QCOMPARE(map.Registers.size(), 2);

    const auto views = map.views();
    QCOMPARE(views.size(), 2);
    QCOMPARE(views[0].Address, quint16(0));
    QCOMPARE(views[0].Length, quint16(1));
    QCOMPARE(views[1].Address, quint16(2));
    QCOMPARE(views[1].Length, quint16(1));
}

///
/// \brief Paths of the library's index that leave the folder are refused.
///
void TestMbMap::keepsDownloadedMapsInsideTheirFolder()
{
    const QString dir = "/data/mbmap-library";
    QCOMPARE(MbMap::cachedFilePath(dir, "acme/em-100.mbmap.json"), QString("/data/mbmap-library/acme/em-100.mbmap.json"));
    QCOMPARE(MbMap::cachedFilePath(dir, "acme/../em-100.mbmap.json"), QString("/data/mbmap-library/em-100.mbmap.json"));

    for(auto&& bad : { "../overwrite.mbmap.json", "acme/../../overwrite.mbmap.json",
                       "/etc/overwrite.mbmap.json", "..\\overwrite.mbmap.json",
                       "C:/overwrite.mbmap.json", "acme/em-100.json", "", ".mbmap.json/../.." })
        QVERIFY2(MbMap::cachedFilePath(dir, bad).isEmpty(), bad);
}

QTEST_GUILESS_MAIN(TestMbMap)
#include "tst_mbmap.moc"
