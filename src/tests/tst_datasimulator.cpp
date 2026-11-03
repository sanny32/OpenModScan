#include <QtTest>
#include "datasimulator.h"

class TestDataSimulator : public QObject
{
    Q_OBJECT
private slots:
    void sequence_data();
    void sequence();
    void reservesRegisters();
    void pauseResumeStop();
    void randomRange();
};

void TestDataSimulator::sequence_data()
{
    QTest::addColumn<SimulationMode>("mode");
    QTest::addColumn<QVector<int>>("expected");
    QTest::newRow("increment-wrap") << SimulationMode::Increment << QVector<int>{2, 4, 6, 2};
    QTest::newRow("decrement-wrap") << SimulationMode::Decrement << QVector<int>{6, 4, 2, 6};
    QTest::newRow("toggle") << SimulationMode::Toggle << QVector<int>{1, 0, 1, 0};
}

void TestDataSimulator::sequence()
{
    QFETCH(SimulationMode, mode);
    QFETCH(QVector<int>, expected);
    DataSimulator sim(nullptr);
    ModbusSimulationParams params(mode);
    params.DataMode = DataDisplayMode::UInt16;
    params.Interval = 20;
    params.IncrementParams.Range = params.DecrementParams.Range = {2., 6.};
    params.IncrementParams.Step = params.DecrementParams.Step = 2.;
    QVector<int> values;
    connect(&sim, &DataSimulator::dataSimulated, this,
            [&](DataDisplayMode, quint8 device, QModbusDataUnit::RegisterType type, quint16 address, QVariant value) {
        QCOMPARE(device, quint8(17));
        QCOMPARE(type, QModbusDataUnit::HoldingRegisters);
        QCOMPARE(address, quint16(100));
        values.append(value.toInt());
    });
    sim.startSimulation(17, QModbusDataUnit::HoldingRegisters, 100, params);
    QTRY_VERIFY_WITH_TIMEOUT(values.size() >= expected.size(), 2000);
    sim.pauseSimulations();
    QCOMPARE(values.mid(0, expected.size()), expected);
}

void TestDataSimulator::reservesRegisters()
{
    DataSimulator sim(nullptr);
    ModbusSimulationParams params(SimulationMode::Random);
    params.DataMode = DataDisplayMode::DblFloat;
    sim.startSimulation(1, QModbusDataUnit::HoldingRegisters, 100, params);
    sim.pauseSimulations();
    for (int addr = 100; addr < 104; ++addr)
        QVERIFY(sim.hasSimulation(1, QModbusDataUnit::HoldingRegisters, addr));
    QVERIFY(!sim.canStartSimulation(DataDisplayMode::Int32, 1, QModbusDataUnit::HoldingRegisters, 99));
    QVERIFY(sim.canStartSimulation(DataDisplayMode::Int32, 2, QModbusDataUnit::HoldingRegisters, 99));
    QVERIFY(sim.canStartSimulation(DataDisplayMode::Int32, 1, QModbusDataUnit::InputRegisters, 99));
    QCOMPARE(sim.simulationMap().size(), 4);
    QSignalSpy stopped(&sim, &DataSimulator::simulationStopped);
    sim.stopSimulation(1, QModbusDataUnit::HoldingRegisters, 100);
    QVERIFY(sim.simulationMap().isEmpty());
    QCOMPARE(stopped.count(), 1);
    QCOMPARE(qvariant_cast<QVector<quint16>>(stopped.first().at(3)), QVector<quint16>({100, 101, 102, 103}));
    sim.stopSimulation(1, QModbusDataUnit::HoldingRegisters, 100);
    QCOMPARE(stopped.count(), 1);
}

void TestDataSimulator::pauseResumeStop()
{
    DataSimulator sim(nullptr);
    ModbusSimulationParams params(SimulationMode::Toggle);
    params.DataMode = DataDisplayMode::Binary;
    params.Interval = 20;
    int count = 0;
    connect(&sim, &DataSimulator::dataSimulated, this, [&] { ++count; });
    sim.startSimulation(1, QModbusDataUnit::Coils, 0, params);
    QTRY_VERIFY_WITH_TIMEOUT(count > 0, 2000);
    sim.pauseSimulations();
    const int paused = count;
    QTest::qWait(60);
    QCOMPARE(count, paused);
    QVERIFY(sim.hasSimulation(1, QModbusDataUnit::Coils, 0));
    sim.resumeSimulations();
    QTRY_VERIFY_WITH_TIMEOUT(count > paused, 2000);
    sim.stopSimulations();
    const int stopped = count;
    QTest::qWait(60);
    QCOMPARE(count, stopped);
    QVERIFY(sim.simulationMap().isEmpty());
}

void TestDataSimulator::randomRange()
{
    DataSimulator sim(nullptr);
    ModbusSimulationParams params(SimulationMode::Random);
    params.DataMode = DataDisplayMode::UInt16;
    params.Interval = 5;
    params.RandomParams.Range = {10., 12.};
    QVector<int> values;
    connect(&sim, &DataSimulator::dataSimulated, this,
            [&](DataDisplayMode, quint8, QModbusDataUnit::RegisterType, quint16, QVariant value) {
        values.append(value.toInt());
    });
    sim.startSimulation(1, QModbusDataUnit::HoldingRegisters, 0, params);
    QTRY_VERIFY_WITH_TIMEOUT(values.size() >= 10, 2000);
    sim.pauseSimulations();
    for (const int value : values)
        QVERIFY(value >= 10 && value <= 12);
}

QTEST_GUILESS_MAIN(TestDataSimulator)
#include "tst_datasimulator.moc"
