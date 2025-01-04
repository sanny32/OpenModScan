#ifndef ASCIIUTILS_H
#define ASCIIUTILS_H

#include <QLocale>
#include <QByteArray>
#include <QTextCodec>
#include "numericutils.h"

///
/// \brief toAscii
/// \param value
/// \param order
/// \return
///
inline QByteArray toAscii(quint16 value, ByteOrder order)
{
    quint8 lo, hi;
    breakUInt16(value, lo, hi, order);

    QByteArray result;
    result.append(hi);
    result.append(lo);

    return result;
}

///
/// \brief fromAscii
/// \param ascii
/// \param order
/// \return
///
inline quint16 fromAscii(const QByteArray& ascii, ByteOrder order)
{
    if(ascii.length() == 2)
        return makeUInt16((quint8)ascii[1], (quint8)ascii[0], order);
    else
        return 0;
}

///
/// \brief printAscii
/// \param data
/// \param sep
/// \return
///
inline QString printAscii(const QByteArray& ascii, const QChar& sep = QChar())
{
    QByteArray result;
    for(auto&& c : ascii)
    {
        const quint8 b = c;
        if(b > 32)
            result.append(b);
        else
            result.append('?');

        if(sep.isPrint())
            result.append(sep.toLatin1());
    }

    return QString::fromLocal8Bit(result);
}

#endif // ASCIIUTILS_H
