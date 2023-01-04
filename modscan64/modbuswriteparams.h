#ifndef MODBUSWRITEPARAMS_H
#define MODBUSWRITEPARAMS_H

#include <QVariant>
#include "enums.h"

struct ModbusWriteParams
{
    quint32 Node;
    quint32 Address;
    QVariant Value;
    DataDisplayMode DisplayMode;
};

#endif // MODBUSWRITEPARAMS_H
