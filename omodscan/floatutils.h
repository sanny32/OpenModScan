#ifndef FLOATUTILS_H
#define FLOATUTILS_H

#include <QtGlobal>
#include <QtEndian>
#include "enums.h"

///
/// \brief breakFloat
/// \param value
/// \param lo
/// \param hi
/// \param order
///
inline void breakFloat(float value, quint16& lo, quint16& hi, ByteOrder order = ByteOrder::LittleEndian)
{
    union {
       quint16 asUint16[2];
       float asFloat;
    } v;
    v.asFloat = value;

    switch(order)
    {
        case ByteOrder::BigEndian:
            lo = qToBigEndian<quint16>(v.asUint16[0]);
            hi = qToBigEndian<quint16>(v.asUint16[1]);
        break;

        case ByteOrder::LittleEndian:
            lo = qToLittleEndian<quint16>(v.asUint16[0]);
            hi = qToLittleEndian<quint16>(v.asUint16[1]);
        break;
    }
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
inline void breakDouble(double value, quint16& lolo, quint16& lohi, quint16& hilo, quint16& hihi, ByteOrder order = ByteOrder::LittleEndian)
{
    union {
       quint16 asUint16[4];
       double asDouble;
    } v;
    v.asDouble = value;

    switch(order)
    {
        case ByteOrder::BigEndian:
            lolo = qToBigEndian<quint16>(v.asUint16[0]);
            lohi = qToBigEndian<quint16>(v.asUint16[1]);
            hilo = qToBigEndian<quint16>(v.asUint16[2]);
            hihi = qToBigEndian<quint16>(v.asUint16[3]);
        break;

        case ByteOrder::LittleEndian:
            lolo = qToLittleEndian<quint16>(v.asUint16[0]);
            lohi = qToLittleEndian<quint16>(v.asUint16[1]);
            hilo = qToLittleEndian<quint16>(v.asUint16[2]);
            hihi = qToLittleEndian<quint16>(v.asUint16[3]);
        break;
    }
}

///
/// \brief makeFloat
/// \param lo
/// \param hi
/// \param order
/// \return
///
inline float makeFloat(quint16 lo, quint16 hi, ByteOrder order = ByteOrder::LittleEndian)
{
    union {
       quint16 asUint16[2];
       float asFloat;
    } v;

    switch(order)
    {
        case ByteOrder::BigEndian:
            lo = qToBigEndian<quint16>(lo);
            hi = qToBigEndian<quint16>(hi);
        break;

        case ByteOrder::LittleEndian:
            lo = qToLittleEndian<quint16>(lo);
            hi = qToLittleEndian<quint16>(hi);
        break;
    }

    v.asUint16[0] = lo;
    v.asUint16[1] = hi;

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
inline double makeDouble(quint16 lolo, quint16 lohi, quint16 hilo, quint16 hihi, ByteOrder order = ByteOrder::LittleEndian)
{
    union {
       quint16 asUint16[4];
       double asDouble;
    } v;

    switch(order)
    {
        case ByteOrder::BigEndian:
            lolo = qToBigEndian<quint16>(lolo);
            lohi = qToBigEndian<quint16>(lohi);
            hilo = qToBigEndian<quint16>(hilo);
            hihi = qToBigEndian<quint16>(hihi);
        break;

        case ByteOrder::LittleEndian:
            lolo = qToLittleEndian<quint16>(lolo);
            lohi = qToLittleEndian<quint16>(lohi);
            hilo = qToLittleEndian<quint16>(hilo);
            hihi = qToLittleEndian<quint16>(hihi);
        break;
    }

    v.asUint16[0] = lolo;
    v.asUint16[1] = lohi;
    v.asUint16[2] = hilo;
    v.asUint16[3] = hihi;

    return v.asDouble;
}

#endif // FLOATUTILS_H
