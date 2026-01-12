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
    quint32 DeviceId;
    quint32 Address;
    QVariant Value;
    DataDisplayMode DisplayMode;
    AddressSpace AddrSpace = AddressSpace::Addr6Digits;
    ByteOrder Order;
    QString Codepage;
    bool ZeroBasedAddress;
    bool LeadingZeros = false;
    bool ForceModbus15And16Func = false;
    ModbusClient* Client = nullptr;
};

///
/// \brief The ModbusMaskWriteParams class
///
struct ModbusMaskWriteParams
{
    quint32 DeviceId;
    quint32 Address;
    quint16 AndMask;
    quint16 OrMask;
    bool ZeroBasedAddress;
    bool LeadingZeros = false;
};

#endif // MODBUSWRITEPARAMS_H
