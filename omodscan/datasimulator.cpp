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
void DataSimulator::startSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const ModbusSimulationParams& params)
{
    QVariant value;
    switch (params.Mode)
    {
        case SimulationMode::Increment:
            value = params.IncrementParams.Range.from();
            emit dataSimulated(mode, type, addr, value);
        break;

        case SimulationMode::Decrement:
            value = params.IncrementParams.Range.to();
            emit dataSimulated(mode, type, addr, value);
        break;

        default:
        break;
    }

    _simulationMap[{ type, addr}] = { mode, params, value };
    resumeSimulations();
}

///
/// \brief DataSimulator::stopSimulation
/// \param type
/// \param addr
///
void DataSimulator::stopSimulation(QModbusDataUnit::RegisterType type, quint16 addr)
{
    _simulationMap.remove({ type, addr});
}

///
/// \brief DataSimulator::stopSimulations
///
void DataSimulator::stopSimulations()
{
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
                randomSimulation(mode, key.first, key.second, params.RandomParams);
            break;

            case SimulationMode::Increment:
                incrementSimulation(mode, key.first, key.second, params.IncrementParams);
            break;

            case SimulationMode::Decrement:
                decrementSimailation(mode, key.first, key.second, params.DecrementParams);
            break;

            case SimulationMode::Toggle:
                toggleSimulation(key.first, key.second);
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
void DataSimulator::randomSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const RandomSimulationParams& params)
{
    auto&& value = _simulationMap[{ type, addr}].CurrentValue;
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
                case DataDisplayMode::Integer:
                case DataDisplayMode::Decimal:
                case DataDisplayMode::Hex:
                    value = generateRandom<quint16>(params.Range.from(), params.Range.to() + 1);
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
        emit dataSimulated(mode, type, addr, value);
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
void DataSimulator::incrementSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const IncrementSimulationParams& params)
{
    auto&& value = _simulationMap[{ type, addr}].CurrentValue;
    switch(mode)
    {
        case DataDisplayMode::Integer:
            value = incrementValue<qint16>(value.toInt(), params.Step, params.Range);
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::Decimal:
        case DataDisplayMode::Hex:
            value = incrementValue<quint16>(value.toUInt(), params.Step, params.Range);
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
        emit dataSimulated(mode, type, addr, value);
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
void DataSimulator::decrementSimailation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const DecrementSimulationParams& params)
{
    auto&& value = _simulationMap[{ type, addr}].CurrentValue;
    switch(mode)
    {
        case DataDisplayMode::Integer:
            value = decrementValue<qint16>(value.toInt(), params.Step, params.Range);
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::Decimal:
        case DataDisplayMode::Hex:
            value = decrementValue<quint16>(value.toUInt(), params.Step, params.Range);
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
        emit dataSimulated(mode, type, addr, value);
}

///
/// \brief DataSimulator::toggleSimulation
/// \param type
/// \param addr
///
void DataSimulator::toggleSimulation(QModbusDataUnit::RegisterType type, quint16 addr)
{
    auto&& value = _simulationMap[{ type, addr}].CurrentValue;
    value = !value.toBool();

    emit dataSimulated(DataDisplayMode::Binary, type, addr, value);
}
