#include <QRandomGenerator>
#include "datasimulator.h"

///
/// \brief DataSimulator::DataSimulator
/// \param parent
///
DataSimulator::DataSimulator(QObject* parent)
    : QObject{parent}
    ,_elapsed(0)
{
    connect(&_timer, &QTimer::timeout, this, &DataSimulator::on_timeout);
}

///
/// \brief DataSimulator::~DataSimulator
///
DataSimulator::~DataSimulator()
{
    stopSimulations();
}

///
/// \brief DataSimulator::startSimulation
/// \param mode
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::startSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, const ModbusSimulationParams& params)
{
    QVariant value;
    switch (params.Mode)
    {
        case SimulationMode::Increment:
            value = params.IncrementParams.Range.from();
            emit dataSimulated(mode, type, addr, deviceId, value);
        break;

        case SimulationMode::Decrement:
            value = params.DecrementParams.Range.to();
            emit dataSimulated(mode, type, addr, deviceId, value);
        break;

        default:
        break;
    }

    _simulationMap[{ type, addr, deviceId}] = { mode, params, value };
    resumeSimulations();

    emit simulationStarted(type, addr, deviceId);
}

///
/// \brief DataSimulator::stopSimulation
/// \param type
/// \param addr
///
void DataSimulator::stopSimulation(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId)
{
    _simulationMap.remove({ type, addr, deviceId});
    emit simulationStopped(type, addr, deviceId);
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
    if(!_timer.isActive())
        _timer.start(_interval);
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
        startSimulation(mode, key.Type, key.Address,  key.DeviceId, params);
    }
}

///
/// \brief DataSimulator::simulationParams
/// \param type
/// \param addr
/// \return
///
ModbusSimulationParams DataSimulator::simulationParams(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId) const
{
    const auto it = _simulationMap.find({type, addr, deviceId});
    return (it != _simulationMap.end()) ? it->Params : ModbusSimulationParams();
}

///
/// \brief DataSimulator::simulationMap
/// \return
///
ModbusSimulationMap DataSimulator::simulationMap(quint8 deviceId) const
{
    ModbusSimulationMap map;
    for(auto&& key : _simulationMap.keys())
        if(key.DeviceId == deviceId)
            map[{key.Type, key.Address}] = _simulationMap[key].Params;

    return map;
}

///
/// \brief DataSimulator::on_timeout
///
void DataSimulator::on_timeout()
{
    _elapsed++;
    for(auto&& key : _simulationMap.keys())
    {
        const auto mode = _simulationMap[key].Mode;
        const auto params = _simulationMap[key].Params;
        const auto interval = params.Interval;

        if(_elapsed % interval) continue;

        switch(params.Mode)
        {
            case SimulationMode::Random:
                randomSimulation(mode, key.Type, key.Address, key.DeviceId, params.RandomParams);
            break;

            case SimulationMode::Increment:
                incrementSimulation(mode, key.Type, key.Address, key.DeviceId, params.IncrementParams);
            break;

            case SimulationMode::Decrement:
                decrementSimailation(mode, key.Type, key.Address, key.DeviceId, params.DecrementParams);
            break;

            case SimulationMode::Toggle:
                toggleSimulation(key.Type, key.Address, key.DeviceId);
            break;

            default:
            break;
        }
    }
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
/// \param type
/// \param addr
/// \param order
/// \param params
///
void DataSimulator::randomSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, const RandomSimulationParams& params)
{
    auto&& value = _simulationMap[{ type, addr, deviceId}].CurrentValue;
    switch(type)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            value = generateRandom<quint16>(params.Range.from(), params.Range.to() + 1);
        break;

        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            switch(mode)
            {
                case DataDisplayMode::Binary:
                case DataDisplayMode::Int16:
                case DataDisplayMode::UInt16:
                case DataDisplayMode::Hex:
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
            }
        break;

        default:
        break;
    }

    if(value.isValid())
        emit dataSimulated(mode, type, addr, deviceId, value);
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
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::incrementSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, const IncrementSimulationParams& params)
{
    auto&& value = _simulationMap[{ type, addr, deviceId}].CurrentValue;
    switch(mode)
    {
        case DataDisplayMode::Int16:
            value = incrementValue<qint16>(value.toInt(), params.Step, params.Range);
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::UInt16:
        case DataDisplayMode::Hex:
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
    }

    if(value.isValid())
        emit dataSimulated(mode, type, addr, deviceId, value);
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
/// \param type
/// \param addr
/// \param order
/// \param params
///
void DataSimulator::decrementSimailation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId, const DecrementSimulationParams& params)
{
    auto&& value = _simulationMap[{ type, addr, deviceId}].CurrentValue;
    switch(mode)
    {
        case DataDisplayMode::Int16:
            value = decrementValue<qint16>(value.toInt(), params.Step, params.Range);
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::UInt16:
        case DataDisplayMode::Hex:
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
    }

    if(value.isValid())
        emit dataSimulated(mode, type, addr, deviceId, value);
}

///
/// \brief DataSimulator::toggleSimulation
/// \param type
/// \param addr
///
void DataSimulator::toggleSimulation(QModbusDataUnit::RegisterType type, quint16 addr, quint8 deviceId)
{
    auto&& value = _simulationMap[{ type, addr, deviceId}].CurrentValue;
    value = !value.toBool();

    emit dataSimulated(DataDisplayMode::Binary, type, addr, deviceId, value);
}
