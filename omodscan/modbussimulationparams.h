#ifndef MODBUSSIMULATIONPARAMS_H
#define MODBUSSIMULATIONPARAMS_H

#include "qrange.h"
#include "enums.h"

///
/// \brief The RandomSimulationParams class
///
struct RandomSimulationParams
{
    QRange<double> Range = QRange<double>(0., 65535.);
};
Q_DECLARE_METATYPE(RandomSimulationParams)

///
/// \brief The IncrementSimulationParams class
///
struct IncrementSimulationParams
{
    double Step = 1.;
    QRange<double> Range = QRange<double>(0., 65535.);
};
Q_DECLARE_METATYPE(IncrementSimulationParams)

///
/// \brief The DecrementSimulationParams class
///
struct DecrementSimulationParams
{
    double Step = 1.;
    QRange<double> Range = QRange<double>(0., 65535.);
};
Q_DECLARE_METATYPE(DecrementSimulationParams)

///
/// \brief The ModbusSimulationParams class
///
struct ModbusSimulationParams
{
    SimulationMode Mode = SimulationMode::No;
    RandomSimulationParams RandomParams;
    IncrementSimulationParams IncrementParams;
    DecrementSimulationParams DecrementParams;
    quint32 Interval = 1;
};
Q_DECLARE_METATYPE(ModbusSimulationParams)

#endif // MODBUSSIMULATIONPARAMS_H
