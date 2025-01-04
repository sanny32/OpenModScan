#ifndef ASCIIUTILS_H
#define ASCIIUTILS_H

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
/// \brief printAscii
/// \param data
/// \param sep
/// \return
///
inline QString printAscii(const QByteArray& ascii, const QChar& sep = ' ')
{
    QByteArray result;
    for(auto&& c : ascii)
    {
        const quint8 b = c;
        if(QChar::isPrint(b) && !QChar::isSpace(b))
            result.append(b);
        else
            result.append(QString("\\x%1").arg(QString::number(b, 16).toUpper(), 2, '0').toLocal8Bit());

        result.append(sep.toLatin1());
    }

    const auto codec = QTextCodec::codecForLocale();
    return codec->toUnicode(result);
}

#endif // ASCIIUTILS_H
