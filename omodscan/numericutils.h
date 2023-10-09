#ifndef NUMERICUTILS_H
#define NUMERICUTILS_H

#include <QtGlobal>
#include <QtEndian>
#include "byteorderutils.h"

///
/// \brief makeWord
/// \param lo
/// \param hi
/// \return
///
inline quint16 makeWord(quint8 lo, quint8 hi, ByteOrder order)
{
    union {
        quint8 asUint8[2];
        quint16 asUint16;
    } v;

    v.asUint8[0] = (order == ByteOrder::LittleEndian) ? lo : hi;
    v.asUint8[1] = (order == ByteOrder::LittleEndian) ? hi : lo;

    return v.asUint16;
}

///
/// \brief breakWord
/// \param value
/// \param lo
/// \param hi
/// \param order
///
inline void breakWord(quint16 value, quint8& lo, quint8& hi, ByteOrder order)
{
    union {
        quint8 asUint8[2];
        quint16 asUint16;
    } v;
    v.asUint16 = value;

    lo = (order == ByteOrder::LittleEndian) ? v.asUint8[0] : v.asUint8[1];
    hi = (order == ByteOrder::LittleEndian) ? v.asUint8[1] : v.asUint8[0];
}

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
/// \brief breakLong
/// \param value
/// \param lo
/// \param hi
/// \param order
///
inline void breakLong(qint32 value, quint16& lo, quint16& hi, ByteOrder order)
{
    union {
       quint16 asUint16[2];
       qint32 asInt32;
    } v;
    v.asInt32 = value;

    lo = toByteOrderValue(v.asUint16[0], order);
    hi = toByteOrderValue(v.asUint16[1], order);
}

///
/// \brief breakULong
/// \param value
/// \param lo
/// \param hi
/// \param order
///
inline void breakULong(quint32 value, quint16& lo, quint16& hi, ByteOrder order)
{
    breakLong((qint32)value, lo, hi, order);
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
/// \brief makeLong
/// \param lo
/// \param hi
/// \param order
/// \return
///
inline qint32 makeLong(quint16 lo, quint16 hi, ByteOrder order)
{
    union {
       quint16 asUint16[2];
       qint32 asInt32;
    } v;

    v.asUint16[0] = toByteOrderValue(lo, order);
    v.asUint16[1] = toByteOrderValue(hi, order);

    return v.asInt32;
}

///
/// \brief makeULong
/// \param lo
/// \param hi
/// \param order
/// \return
///
inline quint32 makeULong(quint16 lo, quint16 hi, ByteOrder order)
{
    return (quint32)makeLong(lo, hi, order);
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

#endif // NUMERICUTILS_H
