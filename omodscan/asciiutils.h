#ifndef ASCIIUTILS_H
#define ASCIIUTILS_H

#include <QLocale>
#include <QByteArray>
#include <QTextCodec>
#include "numericutils.h"

///
/// \brief uint16ToAscii
/// \param value
/// \param order
/// \return
///
inline QByteArray uint16ToAscii(quint16 value, ByteOrder order = ByteOrder::LittleEndian)
{
    quint8 lo, hi;
    breakUInt16(value, lo, hi, order);

    QByteArray result;
    result.append(hi);
    result.append(lo);

    return result;
}

///
/// \brief uint16FromAscii
/// \param ascii
/// \param order
/// \return
///
inline quint16 uint16FromAscii(const QByteArray& ascii, ByteOrder order = ByteOrder::LittleEndian)
{
    if(ascii.length() == 2)
        return makeUInt16((quint8)ascii[1], (quint8)ascii[0], order);
    else
        return 0;
}

///
/// \brief printableAscii
/// \param data
/// \param sep
/// \return
///
inline QString printableAscii(const QByteArray& ascii, const QChar& sep = QChar())
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
