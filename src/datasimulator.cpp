#include <QRandomGenerator>
#include "datasimulator.h"

///
/// \brief DataSimulator::DataSimulator
/// \param parent
///
DataSimulator::DataSimulator(QObject* parent)
    : QObject{parent}
{
    _timer.setSingleShot(true);
    _timer.setTimerType(Qt::PreciseTimer);
    connect(&_timer, &QTimer::timeout, this, &DataSimulator::on_timeout);
    _masterTimer.start();
}

///
/// \brief DataSimulator::~DataSimulator
///
DataSimulator::~DataSimulator()
{
    stopSimulations();
}

///
/// \brief DataSimulator::canStartSimulation
/// \param mode
/// \param deviceId
/// \param type
/// \param addr
/// \return
///
bool DataSimulator::canStartSimulation(DataDisplayMode mode, quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr) const
{
    const auto count = registersCount(mode);

    const auto it = _simulationMap.find({ deviceId, type, addr });
    if(it != _simulationMap.end() && it->Params.Mode != SimulationMode::Disabled)
        return true;

    for(int i = 1; i < count; ++i)
    {
        if(hasSimulation(deviceId, type, addr + i))
            return false;
    }

    return true;
}

///
/// \brief DataSimulator::startSimulation
/// \param deviceId
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::startSimulation(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, const ModbusSimulationParams& params)
{
    QVariant value;
    switch (params.Mode)
    {
    case SimulationMode::Increment:
        value = params.IncrementParams.Range.from();
        emit dataSimulated(params.DataMode, deviceId, type, addr, value);
        break;

    case SimulationMode::Decrement:
        value = params.DecrementParams.Range.to();
        emit dataSimulated(params.DataMode, deviceId, type, addr, value);
        break;

    default:
        break;
    }


    QVector<quint16> addresses;
    for(int i = 0; i < registersCount(params.DataMode); ++i) {
        addresses << addr + i;
    }

    for(int i = 0; i < addresses.length(); ++i) {
        const ModbusSimulationMapKey key = { deviceId, type, addresses[i] };
        ModbusSimulationParams simParams = params;

        if(i != 0) {
            simParams.Mode = SimulationMode::Disabled;
        }

        _simulationMap[key] = { params.DataMode, simParams, value };
    }

    scheduleNextRun();
    emit simulationStarted(params.DataMode, deviceId, type, addresses);
}

///
/// \brief DataSimulator::stopSimulation
/// \param deviceId
/// \param type
/// \param addr
///
void DataSimulator::stopSimulation(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr)
{
    const ModbusSimulationMapKey key = { deviceId, type, addr };

    if(!_simulationMap.contains(key))
        return;

    const auto mode = _simulationMap[key].Mode;

    QVector<quint16> addresses;
    for(int i = 0; i < registersCount(mode); ++i) {
        addresses << addr + i;
    }

    for(auto&& a : addresses) {
        _simulationMap.remove({ deviceId, type, a});
    }

    emit simulationStopped(mode, deviceId, type, addresses);
    scheduleNextRun();
}

///
/// \brief DataSimulator::stopSimulations
///
void DataSimulator::stopSimulations()
{
    pauseSimulations();
    _simulationMap.clear();
}

///
/// \brief DataSimulator::pauseSimulations
///
void DataSimulator::pauseSimulations()
{
    _timer.stop();
}

///
/// \brief DataSimulator::resumeSimulations
///
void DataSimulator::resumeSimulations()
{
   scheduleNextRun();
}

///
/// \brief DataSimulator::restartSimulations
///
void DataSimulator::restartSimulations()
{
    pauseSimulations();
    for(auto&& key : _simulationMap.keys())
    {
        const auto mode = _simulationMap[key].Mode;
        const auto params = _simulationMap[key].Params;
        startSimulation(key.DeviceId, key.Type, key.Address, params);
    }
}

///
/// \brief DataSimulator::simulationParams
/// \param deviceId
/// \param type
/// \param addr
/// \return
///
ModbusSimulationParams DataSimulator::simulationParams(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr) const
{
    const auto it = _simulationMap.find({ deviceId, type, addr});

    if(it == _simulationMap.end()) {
        return ModbusSimulationParams();
    }

    return it->Params;
}

///
/// \brief DataSimulator::hasSimulation
/// \param deviceId
/// \param type
/// \param addr
/// \return
///
bool DataSimulator::hasSimulation(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr) const
{
    const auto mode = simulationParams(deviceId, type, addr).Mode;
    return mode != SimulationMode::Off;
}

///
/// \brief DataSimulator::simulationMap
/// \return
///
ModbusSimulationMap2 DataSimulator::simulationMap() const
{
    ModbusSimulationMap2 map;
    for(auto&& key : _simulationMap.keys())
        map[key] = _simulationMap[key].Params;

    return map;
}

///
/// \brief DataSimulator::scheduleNextRun
///
void DataSimulator::scheduleNextRun()
{
    if (_simulationMap.isEmpty()) {
        _timer.stop();
        return;
    }

    const qint64 currentTime = _masterTimer.elapsed();
    qint64 minNextTime = -1;

    for (auto it = _simulationMap.constBegin(); it != _simulationMap.constEnd(); ++it) {
        const qint64 nextTime = it.value().NextRunTime;

        if (minNextTime == -1 || nextTime < minNextTime) {
            minNextTime = nextTime;
        }
    }

    const qint64 delay = qMax<qint64>(0, minNextTime - currentTime);

    _timer.start(delay);
}

///
/// \brief DataSimulator::on_timeout
///
void DataSimulator::on_timeout()
{
    const qint64 currentTime = _masterTimer.elapsed();

    QMutableMapIterator it(_simulationMap);
    while(it.hasNext())
    {
        it.next();

        auto& sim = it.value();
        const auto& key = it.key();
        const auto mode = sim.Mode;
        const auto params = sim.Params;
        const auto interval = params.Interval;

        if (interval <= 0 ||
            sim.NextRunTime > currentTime ||
            params.Mode == SimulationMode::Disabled ||
            params.Mode == SimulationMode::Off)
        {
            continue;
        }

        switch(params.Mode)
        {
            case SimulationMode::Random:
                randomSimulation(mode, key.DeviceId, key.Type, key.Address, params.RandomParams);
                break;

            case SimulationMode::Increment:
                incrementSimulation(mode, key.DeviceId, key.Type, key.Address, params.IncrementParams);
                break;

            case SimulationMode::Decrement:
                decrementSimailation(mode, key.DeviceId, key.Type, key.Address, params.DecrementParams);
                break;

            case SimulationMode::Toggle:
                toggleSimulation(key.DeviceId, key.Type, key.Address);
                break;

            default:
                break;
        }

        sim.NextRunTime = currentTime + interval;
    }

    scheduleNextRun();
}

template<typename T>
T generateRandom(double from, double to)
{
    return static_cast<T>(from + QRandomGenerator::global()->bounded(to - from));
}

template<typename T>
T generateRandom(const QRange<double>& range)
{
    return generateRandom<T>(range.from(), range.to());
}

///
/// \brief DataSimulator::randomSimulation
/// \param mode
/// \param deviceId
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::randomSimulation(DataDisplayMode mode, quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, const RandomSimulationParams& params)
{
    auto&& value = _simulationMap[{ deviceId, type, addr}].CurrentValue;
    switch(type)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            value = generateRandom<quint16>(0, 2);
        break;

        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            switch(mode)
            {
                case DataDisplayMode::Binary:
                case DataDisplayMode::Int16:
                case DataDisplayMode::UInt16:
                case DataDisplayMode::Hex:
                case DataDisplayMode::Ansi:
                    value = generateRandom<quint16>(params.Range.from(), params.Range.to() + 1);
                break;

                case DataDisplayMode::Int32:
                case DataDisplayMode::SwappedInt32:
                    value = generateRandom<qint32>(params.Range);
                break;

                case DataDisplayMode::UInt32:
                case DataDisplayMode::SwappedUInt32:
                    value = generateRandom<quint32>(params.Range);
                break;

                case DataDisplayMode::FloatingPt:
                case DataDisplayMode::SwappedFP:
                   value = generateRandom<float>(params.Range);
                break;

                case DataDisplayMode::DblFloat:
                case DataDisplayMode::SwappedDbl:
                   value = generateRandom<double>(params.Range);
                break;

                case DataDisplayMode::Int64:
                case DataDisplayMode::SwappedInt64:
                    value = generateRandom<qint64>(params.Range);
                break;

                case DataDisplayMode::UInt64:
                case DataDisplayMode::SwappedUInt64:
                    value = generateRandom<quint64>(params.Range);
                break;
            }
        break;

        default:
        break;
    }

    if(value.isValid())
        emit dataSimulated(mode, deviceId, type, addr, value);
}

template<typename T>
T incrementValue(T value, T step, const QRange<double>& range)
{
    value = value + step;
    if(value > range.to() || value < range.from()) value = static_cast<T>(range.from());
    return value;
}

///
/// \brief DataSimulator::incrementSimulation
/// \param mode
/// \param deviceId
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::incrementSimulation(DataDisplayMode mode, quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, const IncrementSimulationParams& params)
{
    auto&& value = _simulationMap[{ deviceId, type, addr}].CurrentValue;
    switch(mode)
    {
        case DataDisplayMode::Int16:
            value = incrementValue<qint16>(value.toInt(), params.Step, params.Range);
            break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::UInt16:
        case DataDisplayMode::Hex:
        case DataDisplayMode::Ansi:
            value = incrementValue<quint16>(value.toUInt(), params.Step, params.Range);
            break;

        case DataDisplayMode::Int32:
        case DataDisplayMode::SwappedInt32:
            value = incrementValue<qint32>(value.toInt(),  params.Step, params.Range);
            break;

        case DataDisplayMode::UInt32:
        case DataDisplayMode::SwappedUInt32:
            value = incrementValue<quint32>(value.toUInt(),  params.Step, params.Range);
            break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            value = incrementValue<float>(value.toFloat(), params.Step, params.Range);
            break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            value = incrementValue<double>(value.toDouble(), params.Step, params.Range);
            break;

        case DataDisplayMode::Int64:
        case DataDisplayMode::SwappedInt64:
            value = incrementValue<qint64>(value.toLongLong(), params.Step, params.Range);
            break;

        case DataDisplayMode::UInt64:
        case DataDisplayMode::SwappedUInt64:
            value = incrementValue<quint64>(value.toULongLong(), params.Step, params.Range);
            break;
    }

    if(value.isValid())
        emit dataSimulated(mode, deviceId, type, addr, value);
}

template<typename T>
T decrementValue(T value, T step, const QRange<double>& range)
{
    value = value - step;
    if(value > range.to() || value < range.from()) value = static_cast<T>(range.to());
    return value;
}

///
/// \brief DataSimulator::decrementSimailation
/// \param mode
/// \param deviceId
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::decrementSimailation(DataDisplayMode mode, quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr, const DecrementSimulationParams& params)
{
    auto&& value = _simulationMap[{ deviceId, type, addr}].CurrentValue;
    switch(mode)
    {
        case DataDisplayMode::Int16:
            value = decrementValue<qint16>(value.toInt(), params.Step, params.Range);
            break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::UInt16:
        case DataDisplayMode::Hex:
        case DataDisplayMode::Ansi:
            value = decrementValue<quint16>(value.toUInt(), params.Step, params.Range);
            break;

        case DataDisplayMode::Int32:
        case DataDisplayMode::SwappedInt32:
            value = decrementValue<qint32>(value.toInt(),  params.Step, params.Range);
            break;

        case DataDisplayMode::UInt32:
        case DataDisplayMode::SwappedUInt32:
            value = decrementValue<quint32>(value.toUInt(),  params.Step, params.Range);
            break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            value = decrementValue<float>(value.toFloat(), params.Step, params.Range);
            break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            value = decrementValue<double>(value.toDouble(), params.Step, params.Range);
            break;

        case DataDisplayMode::Int64:
        case DataDisplayMode::SwappedInt64:
            value = decrementValue<qint64>(value.toLongLong(), params.Step, params.Range);
            break;

        case DataDisplayMode::UInt64:
        case DataDisplayMode::SwappedUInt64:
            value = decrementValue<quint64>(value.toULongLong(), params.Step, params.Range);
            break;
    }

    if(value.isValid())
        emit dataSimulated(mode, deviceId, type, addr, value);
}

///
/// \brief DataSimulator::toggleSimulation
/// \param deviceId
/// \param type
/// \param addr
///
void DataSimulator::toggleSimulation(quint8 deviceId, QModbusDataUnit::RegisterType type, quint16 addr)
{
    auto&& value = _simulationMap[{ deviceId, type, addr}].CurrentValue;
    value = !value.toBool();

    emit dataSimulated(DataDisplayMode::Binary, deviceId, type, addr, value);
}
