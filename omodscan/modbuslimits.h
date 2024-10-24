#ifndef MODBUSLIMITS_H
#define MODBUSLIMITS_H

#include "qrange.h"

class ModbusLimits final
{
public:
    static QRange<int> addressRange(bool zeroBased = false)  { return { (zeroBased ? 0 : 1), 65535 }; }
    static QRange<int> lengthRange()   { return { 1, 125   }; }
    static QRange<int> slaveRange()    { return { 1, 255   }; }
};

#endif // MODBUSLIMITS_H
