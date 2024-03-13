#ifndef NUMERICUTILS_H
#define NUMERICUTILS_H

#include <QtGlobal>
#include <QtEndian>
#include "byteorderutils.h"

///
/// \brief makeUInt16
/// \param lo
/// \param hi
/// \return
///
inline quint16 makeUInt16(quint8 lo, quint8 hi, ByteOrder order)
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
/// \brief breakUInt16
/// \param value
/// \param lo
/// \param hi
/// \param order
///
inline void breakUInt16(quint16 value, quint8& lo, quint8& hi, ByteOrder order)
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
/// \brief breakInt32
/// \param value
/// \param lo
/// \param hi
/// \param order
///
inline void breakInt32(qint32 value, quint16& lo, quint16& hi, ByteOrder order)
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
/// \brief breakUInt32
/// \param value
/// \param lo
/// \param hi
/// \param order
///
inline void breakUInt32(quint32 value, quint16& lo, quint16& hi, ByteOrder order)
{
    breakInt32((qint32)value, lo, hi, order);
}

///
/// \brief breakInt64
/// \param value
/// \param lolo
/// \param lohi
/// \param hilo
/// \param hihi
/// \param order
///
inline void breakInt64(qint64 value, quint16& lolo, quint16& lohi, quint16& hilo, quint16& hihi, ByteOrder order)
{
    union {
        quint16 asUint16[4];
        qint64 asInt64;
    } v;
    v.asInt64 = value;

    lolo = toByteOrderValue(v.asUint16[0], order);
    lohi = toByteOrderValue(v.asUint16[1], order);
    hilo = toByteOrderValue(v.asUint16[2], order);
    hihi = toByteOrderValue(v.asUint16[3], order);
}

///
/// \brief breakUInt64
/// \param value
/// \param lolo
/// \param lohi
/// \param hilo
/// \param hihi
/// \param order
///
inline void breakUInt64(quint64 value, quint16& lolo, quint16& lohi, quint16& hilo, quint16& hihi, ByteOrder order)
{
     breakInt64((qint64)value, lolo, lohi, hilo, hihi, order);
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
/// \brief makeInt32
/// \param lo
/// \param hi
/// \param order
/// \return
///
inline qint32 makeInt32(quint16 lo, quint16 hi, ByteOrder order)
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
/// \brief makeUInt32
/// \param lo
/// \param hi
/// \param order
/// \return
///
inline quint32 makeUInt32(quint16 lo, quint16 hi, ByteOrder order)
{
    return (quint32)makeInt32(lo, hi, order);
}

///
/// \brief makeInt64
/// \param lolo
/// \param lohi
/// \param hilo
/// \param hihi
/// \param order
/// \return
///
inline qint64 makeInt64(quint16 lolo, quint16 lohi, quint16 hilo, quint16 hihi, ByteOrder order)
{
    union {
        quint16 asUint16[4];
        qint64 asInt64;
    } v;

    v.asUint16[0] = toByteOrderValue(lolo, order);
    v.asUint16[1] = toByteOrderValue(lohi, order);
    v.asUint16[2] = toByteOrderValue(hilo, order);
    v.asUint16[3] = toByteOrderValue(hihi, order);

    return v.asInt64;
}

///
/// \brief makeUInt64
/// \param lolo
/// \param lohi
/// \param hilo
/// \param hihi
/// \param order
/// \return
///
inline qint64 makeUInt64(quint16 lolo, quint16 lohi, quint16 hilo, quint16 hihi, ByteOrder order)
{
     return (quint64)makeInt64(lolo, lohi, hilo, hihi, order);
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
