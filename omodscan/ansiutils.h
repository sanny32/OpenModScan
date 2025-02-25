#ifndef ANSIUTILS_H
#define ANSIUTILS_H

#include <QLocale>
#include <QByteArray>
#include <QTextCodec>
#include <QDebug>
#include "numericutils.h"

///
/// \brief uint16ToAnsi
/// \param value
/// \param order
/// \return
///
inline QByteArray uint16ToAnsi(quint16 value, ByteOrder order = ByteOrder::LittleEndian)
{
    quint8 lo, hi;
    breakUInt16(value, lo, hi, order);

    QByteArray result;
    result.append(hi);
    result.append(lo);

    return result;
}

///
/// \brief uint16FromAnsi
/// \param Ansi
/// \param order
/// \return
///
inline quint16 uint16FromAnsi(const QByteArray& Ansi, ByteOrder order = ByteOrder::LittleEndian)
{
    if(Ansi.length() == 2)
        return makeUInt16((quint8)Ansi[1], (quint8)Ansi[0], order);
    else
        return 0;
}

///
/// \brief printableAnsi
/// \param data
/// \param sep
/// \return
///
inline QString printableAnsi(const QByteArray& Ansi, const QChar& sep = QChar())
{
    QByteArray result;
    for(auto&& c : Ansi)
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

#endif // ANSIUTILS_H
