#ifndef BYTEORDERUTILS_H
#define BYTEORDERUTILS_H

#include <QtEndian>
#include "enums.h"

///
/// \brief toByteOrderValue
/// \param value
/// \param order
/// \return
///
template<typename T>
inline T toByteOrderValue(T value, ByteOrder order)
{
    switch(order)
    {
        case ByteOrder::Swapped:
            return qToBigEndian<T>(value);

        case ByteOrder::Direct:
            return qToLittleEndian<T>(value);
    }

    return value;
}

#endif // BYTEORDERUTILS_H
