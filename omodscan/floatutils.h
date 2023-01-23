#ifndef FLOATUTILS_H
#define FLOATUTILS_H

#include <QtGlobal>

///
/// \brief breakFloat
/// \param value
/// \param lo
/// \param hi
///
inline void breakFloat(float value, quint16& lo, quint16& hi)
{
    union {
       quint16 asUint16[2];
       float asFloat;
    } v;
    v.asFloat = value;
    lo = v.asUint16[0];
    hi = v.asUint16[1];
}

///
/// \brief breakDouble
/// \param value
/// \param lolo
/// \param lohi
/// \param hilo
/// \param hihi
///
inline void breakDouble(double value, quint16& lolo, quint16& lohi, quint16& hilo, quint16& hihi)
{
    union {
       quint16 asUint16[4];
       double asDouble;
    } v;
    v.asDouble = value;
    lolo = v.asUint16[0];
    lohi = v.asUint16[1];
    hilo = v.asUint16[2];
    hihi = v.asUint16[3];
}

///
/// \brief makeFloat
/// \param lo
/// \param hi
/// \return
///
inline float makeFloat(quint16 lo, quint16 hi)
{
    union {
       quint16 asUint16[2];
       float asFloat;
    } v;
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
/// \return
///
inline double makeDouble(quint16 lolo, quint16 lohi, quint16 hilo, quint16 hihi)
{
    union {
       quint16 asUint16[4];
       double asDouble;
    } v;
    v.asUint16[0] = lolo;
    v.asUint16[1] = lohi;
    v.asUint16[2] = hilo;
    v.asUint16[3] = hihi;

    return v.asDouble;
}

#endif // FLOATUTILS_H
