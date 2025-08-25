#ifndef MODBUSWRITEPARAMS_H
#define MODBUSWRITEPARAMS_H

#include <QVariant>
#include "enums.h"

class ModbusClient;

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
    QString Codepage;
    bool ZeroBasedAddress;
    ModbusClient* Client = nullptr;
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
