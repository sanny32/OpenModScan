#ifndef MODBUSSIMULATIONPARAMS_H
#define MODBUSSIMULATIONPARAMS_H

#include <QDataStream>
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
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const RandomSimulationParams& params)
{
    out << params.Range;
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, RandomSimulationParams& params)
{
    in >> params.Range;
    return in;
}

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
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const IncrementSimulationParams& params)
{
    out << params.Step;
    out << params.Range;
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, IncrementSimulationParams& params)
{
    in >> params.Step;
    in >> params.Range;
    return in;
}

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
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const DecrementSimulationParams& params)
{
    out << params.Step;
    out << params.Range;
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, DecrementSimulationParams& params)
{
    in >> params.Step;
    in >> params.Range;
    return in;
}

///
/// \brief The ModbusSimulationParams class
///
struct ModbusSimulationParams
{
    SimulationMode Mode = SimulationMode::No;
    RandomSimulationParams RandomParams;
    IncrementSimulationParams IncrementParams;
    DecrementSimulationParams DecrementParams;
    quint32 Interval = 1000;
};
Q_DECLARE_METATYPE(ModbusSimulationParams)

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const ModbusSimulationParams& params)
{
    out << params.Mode;
    out << params.RandomParams;
    out << params.IncrementParams;
    out << params.DecrementParams;
    out << params.Interval;

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, ModbusSimulationParams& params)
{
    in >> params.Mode;
    in >> params.RandomParams;
    in >> params.IncrementParams;
    in >> params.DecrementParams;
    in >> params.Interval;
    return in;
}

#endif // MODBUSSIMULATIONPARAMS_H
