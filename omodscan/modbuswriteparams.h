#ifndef MODBUSWRITEPARAMS_H
#define MODBUSWRITEPARAMS_H

#include <QVariant>
#include "enums.h"

///
/// \brief The ModbusWriteParams class
///
struct ModbusWriteParams
{
    quint32 Node;
    quint32 Address;
    QVariant Value;
    DataDisplayMode DisplayMode;
    ByteOrder Order;
    bool ZeroBasedAddress;
};

///
/// \brief The ModbusMaskWriteParams class
///
struct ModbusMaskWriteParams
{
    quint32 Node;
    quint32 Address;
    quint16 AndMask;
    quint16 OrMask;
    bool ZeroBasedAddress;
};

#endif // MODBUSWRITEPARAMS_H
