#ifndef MODBUSLIMITS_H
#define MODBUSLIMITS_H

#include "qrange.h"

class ModbusLimits final
{
public:
    static QRange<int> addressRange(bool zeroBased = false)  { return { 1 - zeroBased, 65536 - zeroBased }; }
    static QRange<int> lengthRange()   { return { 1, 125   }; }
    static QRange<int> lengthRange(int address, bool zeroBased) {
        const int offset = address - (zeroBased ? 0 : 1);
        const int maxLen = qMin(lengthRange().to(), 65536 - offset);
        return { 1, qMax(1, maxLen) };
    }
    static QRange<int> slaveRange()    { return { 1, 255   }; }
};

#endif // MODBUSLIMITS_H
