#ifndef DISPLAYDEFINITION_H
#define DISPLAYDEFINITION_H

#include <QSettings>
#include <QModbusDataUnit>
#include "modbuslimits.h"

///
/// \brief The DisplayDefinition struct
///
struct DisplayDefinition
{
    quint32 ScanRate = 1000;
    quint8 DeviceId = 1;
    quint16 PointAddress = 1;
    QModbusDataUnit::RegisterType PointType = QModbusDataUnit::Coils;
    quint16 Length = 50;
    quint16 LogViewLimit = 30;

    void normalize()
    {
        ScanRate = qBound(20U, ScanRate, 10000U);
        DeviceId = qMax<quint8>(ModbusLimits::slaveRange().from(), DeviceId);
        PointAddress = qMax<quint16>(ModbusLimits::addressRange().from(), PointAddress);
        PointType = qBound(QModbusDataUnit::DiscreteInputs, PointType, QModbusDataUnit::HoldingRegisters);
        Length = qBound<quint16>(ModbusLimits::lengthRange().from(), Length, ModbusLimits::lengthRange().to());
        LogViewLimit = qBound<quint16>(4, LogViewLimit, 1000);
    }
};
Q_DECLARE_METATYPE(DisplayDefinition)

inline QSettings& operator <<(QSettings& out, const DisplayDefinition& dd)
{
    out.setValue("DisplayDefinition/ScanRate",      dd.ScanRate);
    out.setValue("DisplayDefinition/DeviceId",      dd.DeviceId);
    out.setValue("DisplayDefinition/PointAddress",  dd.PointAddress);
    out.setValue("DisplayDefinition/PointType",     dd.PointType);
    out.setValue("DisplayDefinition/Length",        dd.Length);
    out.setValue("DisplayDefinition/LogViewLimit",  dd.LogViewLimit);

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param dd
/// \return
///
inline QSettings& operator >>(QSettings& in, DisplayDefinition& dd)
{
    dd.ScanRate = in.value("DisplayDefinition/ScanRate", 1000).toUInt();
    dd.DeviceId = in.value("DisplayDefinition/DeviceId", 1).toUInt();
    dd.PointAddress = in.value("DisplayDefinition/PointAddress", 1).toUInt();
    dd.PointType = (QModbusDataUnit::RegisterType)in.value("DisplayDefinition/PointType", 1).toUInt();
    dd.Length = in.value("DisplayDefinition/Length", 50).toUInt();
    dd.LogViewLimit = in.value("DisplayDefinition/LogViewLimit", 30).toUInt();

    dd.normalize();
    return in;
}

#endif // DISPLAYDEFINITION_H
