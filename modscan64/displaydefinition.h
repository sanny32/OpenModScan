#ifndef DISPLAYDEFINITION_H
#define DISPLAYDEFINITION_H

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
};

#endif // DISPLAYDEFINITION_H
