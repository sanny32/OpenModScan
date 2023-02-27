#include <QRandomGenerator>
#include "floatutils.h"
#include "datasimulator.h"
#include "formmodsca.h"

///
/// \brief DataSimulator::DataSimulator
/// \param client
///
DataSimulator::DataSimulator(FormModSca* form)
    : QObject{form}
    ,_form(form)
    ,_elapsed(0)
{
    Q_ASSERT(_form != nullptr);
    connect(&_timer, &QTimer::timeout, this, &DataSimulator::on_timeout);
    _timer.start(1000);
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
    switch (params.Mode)
    {
        case SimulationMode::Increment:
            initializeValue(mode, type, addr, params.IncrementParams.Range.from());
        break;

        case SimulationMode::Decrement:
            initializeValue(mode, type, addr, params.IncrementParams.Range.to());
        break;

        default:
        break;
    }

    _simulationMap[{ type, addr}] = { mode, params };
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
/// \brief DataSimulator::on_timeout
///
void DataSimulator::on_timeout()
{
    _elapsed++;
    for(auto&& key : _simulationMap.keys())
    {
        const auto mode = _simulationMap[key].first;
        const auto params = _simulationMap[key].second;
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

///
/// \brief DataSimulator::initializeValue
/// \param mode
/// \param type
/// \param addr
/// \param value
///
void DataSimulator::initializeValue(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, double value)
{
    switch(mode)
    {
        case DataDisplayMode::Integer:
            emit dataSimulated(type, addr, static_cast<qint16>(value));
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::Decimal:
        case DataDisplayMode::Hex:
            emit dataSimulated(type, addr, static_cast<quint16>(value));
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            emit dataSimulated(type, addr, static_cast<float>(value));
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            emit dataSimulated(type, addr, value);
        break;
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
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::randomSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const RandomSimulationParams& params)
{
    QVector<quint16> values;
    switch(type)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            values.push_back(generateRandom<quint16>(params.Range.from(), params.Range.to() + 1));
        break;

        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            switch(mode)
            {
                case DataDisplayMode::Binary:
                case DataDisplayMode::Integer:
                case DataDisplayMode::Decimal:
                case DataDisplayMode::Hex:
                {
                    const auto value = generateRandom<quint16>(params.Range.from(), params.Range.to() + 1);
                    values.push_back(toByteOrderValue(value, _form->byteOrder()));
                }
                break;

                case DataDisplayMode::FloatingPt:
                    values.resize(2);
                    breakFloat(generateRandom<float>(params.Range), values[0], values[1], _form->byteOrder());
                break;

                case DataDisplayMode::SwappedFP:
                    values.resize(2);
                    breakFloat(generateRandom<float>(params.Range), values[1], values[0], _form->byteOrder());
                break;

                case DataDisplayMode::DblFloat:
                    values.resize(4);
                    breakDouble(generateRandom<double>(params.Range), values[0], values[1], values[2], values[3], _form->byteOrder());
                break;

                case DataDisplayMode::SwappedDbl:
                    values.resize(4);
                    breakDouble(generateRandom<double>(params.Range), values[3], values[2], values[1], values[0], _form->byteOrder());
                break;
            }
        break;

        default:
        break;
    }

    if(!values.isEmpty())
    {
        emit dataSimulated(type, addr, QVariant::fromValue(values));
    }
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
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::incrementSimulation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const IncrementSimulationParams& params)
{
    switch(mode)
    {
        case DataDisplayMode::Integer:
            emit dataSimulated(type, addr, incrementValue<qint16>(_form->data(addr), params.Step, params.Range));
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::Decimal:
        case DataDisplayMode::Hex:
            emit dataSimulated(type, addr, incrementValue<quint16>(_form->data(addr), params.Step, params.Range));
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            emit dataSimulated(type, addr, incrementValue<float>(_form->getFloat(addr), params.Step, params.Range));
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            emit dataSimulated(type, addr, incrementValue<double>(_form->getDouble(addr), params.Step, params.Range));
        break;
    }
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
/// \param type
/// \param addr
/// \param params
///
void DataSimulator::decrementSimailation(DataDisplayMode mode, QModbusDataUnit::RegisterType type, quint16 addr, const DecrementSimulationParams& params)
{
    switch(mode)
    {
        case DataDisplayMode::Integer:
            emit dataSimulated(type, addr, decrementValue<qint16>(_form->data(addr), params.Step, params.Range));
        break;

        case DataDisplayMode::Binary:
        case DataDisplayMode::Decimal:
        case DataDisplayMode::Hex:
            emit dataSimulated(type, addr, decrementValue<quint16>(_form->data(addr), params.Step, params.Range));
        break;

        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            emit dataSimulated(type, addr, decrementValue<float>(_form->getFloat(addr), params.Step, params.Range));
        break;

        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            emit dataSimulated(type, addr, decrementValue<double>(_form->getDouble(addr), params.Step, params.Range));
        break;
    }
}

///
/// \brief DataSimulator::toggleSimulation
/// \param type
/// \param addr
///
void DataSimulator::toggleSimulation(QModbusDataUnit::RegisterType type, quint16 addr)
{
    emit dataSimulated(type, addr, !_form->data(addr));
}
