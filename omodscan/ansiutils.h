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
inline QByteArray uint16ToAnsi(quint16 value, ByteOrder order = ByteOrder::Direct)
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
inline quint16 uint16FromAnsi(const QByteArray& ansi, ByteOrder order = ByteOrder::Direct)
{
    if(ansi.length() == 2)
        return makeUInt16((quint8)ansi[1], (quint8)ansi[0], order);
    else
        return 0;
}

///
/// \brief printableAnsi
/// \param ansi
/// \param codepage
/// \param sep
/// \return
///
inline QString printableAnsi(const QByteArray& ansi, const QString& codepage, const QChar& sep = QChar())
{
    QByteArray result;
    for(auto&& c : ansi)
    {
        const quint8 b = c;
        if(b >= 32)
            result.append(b);
        else
            result.append('?');

        if(sep.isPrint())
            result.append(sep.toLatin1());
    }

    auto codec = QTextCodec::codecForName(codepage.toUtf8());
    return codec ? codec->toUnicode(result) : QString::fromLocal8Bit(result);
}

#endif // ANSIUTILS_H
