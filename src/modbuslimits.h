#ifndef MODBUSLIMITS_H
#define MODBUSLIMITS_H

#include "enums.h"
#include "qrange.h"

class ModbusLimits final
{
public:
    static int addressSpaceSize(AddressSpace space) {
        return (space == AddressSpace::Addr5Digits) ? 9999 : 65536;
    }
    static QRange<int> addressRange(AddressSpace space, bool zeroBased = false) {
        const int size = addressSpaceSize(space);
        return { 1 - zeroBased, size - zeroBased };
    }
    static QRange<int> lengthRange()   { return { 1, 125   }; }
    static QRange<int> lengthRange(int address, bool zeroBased, AddressSpace space) {
        const int offset = address - (zeroBased ? 0 : 1);
        const int maxLen = qMin(lengthRange().to(), addressSpaceSize(space) - offset);
        return { 1, qMax(1, maxLen) };
    }
    static QRange<int> slaveRange()    { return { 0, 255   }; }
};

#endif // MODBUSLIMITS_H
