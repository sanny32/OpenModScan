#ifndef MODBUSWRITEPARAMS_H
#define MODBUSWRITEPARAMS_H

#include <QVariant>

struct ModbusWriteParams
{
    quint32 Node;
    quint32 Address;
    QVariant Value;
};

#endif // MODBUSWRITEPARAMS_H
