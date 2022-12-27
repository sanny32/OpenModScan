#ifndef DISPLAYDEFINITION_H
#define DISPLAYDEFINITION_H

#include <QtGlobal>
#include <QModbusDataUnit>

///
/// \brief The DisplayDefinition struct
///
struct DisplayDefinition
{
    uint ScanRate = 1000;
    uchar DeviceId = 1;
    ushort PointAddress = 1;
    QModbusDataUnit::RegisterType PointType = QModbusDataUnit::Coils;
    uchar Length = 100;
};

#endif // DISPLAYDEFINITION_H
