#ifndef FLOATUTILS_H
#define FLOATUTILS_H

#include <QtGlobal>
#include <QtEndian>
#include "byteorderutils.h"

///
/// \brief breakFloat
/// \param value
/// \param lo
/// \param hi
/// \param order
///
inline void breakFloat(float value, quint16& lo, quint16& hi, ByteOrder order)
{
    union {
       quint16 asUint16[2];
       float asFloat;
    } v;
    v.asFloat = value;

    lo = toByteOrderValue(v.asUint16[0], order);
    hi = toByteOrderValue(v.asUint16[1], order);
}

///
/// \brief breakDouble
/// \param value
/// \param lolo
/// \param lohi
/// \param hilo
/// \param hihi
/// \param order
///
inline void breakDouble(double value, quint16& lolo, quint16& lohi, quint16& hilo, quint16& hihi, ByteOrder order)
{
    union {
       quint16 asUint16[4];
       double asDouble;
    } v;
    v.asDouble = value;

    lolo = toByteOrderValue(v.asUint16[0], order);
    lohi = toByteOrderValue(v.asUint16[1], order);
    hilo = toByteOrderValue(v.asUint16[2], order);
    hihi = toByteOrderValue(v.asUint16[3], order);
}

///
/// \brief makeFloat
/// \param lo
/// \param hi
/// \param order
/// \return
///
inline float makeFloat(quint16 lo, quint16 hi, ByteOrder order)
{
    union {
       quint16 asUint16[2];
       float asFloat;
    } v;

    v.asUint16[0] = toByteOrderValue(lo, order);
    v.asUint16[1] = toByteOrderValue(hi, order);

    return v.asFloat;
}

///
/// \brief makeDouble
/// \param lolo
/// \param lohi
/// \param hilo
/// \param hihi
/// \param order
/// \return
///
inline double makeDouble(quint16 lolo, quint16 lohi, quint16 hilo, quint16 hihi, ByteOrder order)
{
    union {
       quint16 asUint16[4];
       double asDouble;
    } v;

    v.asUint16[0] = toByteOrderValue(lolo, order);
    v.asUint16[1] = toByteOrderValue(lohi, order);
    v.asUint16[2] = toByteOrderValue(hilo, order);
    v.asUint16[3] = toByteOrderValue(hihi, order);

    return v.asDouble;
}

#endif // FLOATUTILS_H
