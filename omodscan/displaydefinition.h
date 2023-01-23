#ifndef DISPLAYDEFINITION_H
#define DISPLAYDEFINITION_H

#include <QSettings>
#include <QModbusDataUnit>

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

    void normalize()
    {
        ScanRate = qBound(20U, ScanRate, 10000U);
        DeviceId = qBound<quint8>(1, DeviceId, 255);
        PointAddress = qMax<quint16>(1U, PointAddress);
        PointType = qBound(QModbusDataUnit::DiscreteInputs, PointType, QModbusDataUnit::HoldingRegisters);
        Length = qBound<quint16>(1, Length, 128);
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
    dd.ScanRate = in.value("DisplayDefinition/ScanRate").toUInt();
    dd.DeviceId = in.value("DisplayDefinition/DeviceId").toUInt();
    dd.PointAddress = in.value("DisplayDefinition/PointAddress").toUInt();
    dd.PointType = (QModbusDataUnit::RegisterType)in.value("DisplayDefinition/PointType").toUInt();
    dd.Length = in.value("DisplayDefinition/Length").toUInt();

    dd.normalize();
    return in;
}

#endif // DISPLAYDEFINITION_H
