#ifndef DISPLAYDEFINITION_H
#define DISPLAYDEFINITION_H

#include <QtGlobal>

///
/// \brief The DisplayDefinition struct
///
struct DisplayDefinition
{
    uint ScanRate = 1000;
    uchar DeviceId = 1;
    ushort PointAddress = 1;
    uint PointType = 1;
    uchar Length = 100;
};

#endif // DISPLAYDEFINITION_H
