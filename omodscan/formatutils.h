#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include <QString>
#include <QLocale>
#include <QModbusPdu>
#include <QModbusDataUnit>
#include "enums.h"
#include "numericutils.h"
#include "byteorderutils.h"


///
/// \brief formatUInt8Value
/// \param mode
/// \param c
/// \return
///
inline QString formatUInt8Value(DataDisplayMode mode, quint8 c)
{
    switch(mode)
    {
    case DataDisplayMode::UInt16:
        case DataDisplayMode::Int16:
            return QString("%1").arg(QString::number(c), 3, '0');

        default:
            return QString("0x%1").arg(QString::number(c, 16).toUpper(), 2, '0');
    }
}

///
/// \brief formatUInt8Array
/// \param mode
/// \param ar
/// \return
///
inline QString formatUInt8Array(DataDisplayMode mode, const QByteArray& ar)
{
    QStringList values;
    for(quint8 i : ar)
        switch(mode)
        {
        case DataDisplayMode::UInt16:
            case DataDisplayMode::Int16:
                values += QString("%1").arg(QString::number(i), 3, '0');
            break;

            default:
                values += QString("%1").arg(QString::number(i, 16).toUpper(), 2, '0');
            break;
        }

    return values.join(" ");
}

///
/// \brief formatUInt16Array
/// \param mode
/// \param ar
/// \param order
/// \return
///
inline QString formatUInt16Array(DataDisplayMode mode, const QByteArray& ar, ByteOrder order)
{
    QStringList values;
    for(int i = 0; i < ar.size(); i+=2)
    {
        const quint16 value = makeUInt16(ar[i+1], ar[i], order);
        switch(mode)
        {
        case DataDisplayMode::UInt16:
            case DataDisplayMode::Int16:
                values += QString("%1").arg(QString::number(value), 5, '0');
                break;

            default:
                values += QString("0x%1").arg(QString::number(value, 16).toUpper(), 4, '0');
                break;
        }
    }

    return values.join(" ");
}

///
/// \brief formatUInt16Value
/// \param mode
/// \param v
/// \return
///
inline QString formatUInt16Value(DataDisplayMode mode, quint16 v)
{
    switch(mode)
    {
    case DataDisplayMode::UInt16:
        case DataDisplayMode::Int16:
            return QString("%1").arg(QString::number(v), 5, '0');

        default:
            return QString("0x%1").arg(QString::number(v, 16).toUpper(), 4, '0');
    }
}

///
/// \brief formatBinaryValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
inline QString formatBinaryValue(QModbusDataUnit::RegisterType pointType, quint16 value, ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 16, 2, QLatin1Char('0'));
            break;
        default:
            break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatUInt16Value
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
inline QString formatUInt16Value(QModbusDataUnit::RegisterType pointType, quint16 value, ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QStringLiteral("<%1>").arg(value, 1, 10, QLatin1Char('0'));
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 5, 10, QLatin1Char('0'));
            break;
        default:
            break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatInt16Value
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
inline QString formatInt16Value(QModbusDataUnit::RegisterType pointType, qint16 value, ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QStringLiteral("<%1>").arg(value, 5, 10, QLatin1Char(' '));
            break;
        default:
            break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatHexValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
inline QString formatHexValue(QModbusDataUnit::RegisterType pointType, quint16 value, ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
            result = QString("<0x%1>").arg(QString::number(value, 16).toUpper(), 4, '0');
            break;
        default:
            break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatAsciiValue
/// \param pointType
/// \param value
/// \param order
/// \param outValue
/// \return
///
inline QString formatAsciiValue(QModbusDataUnit::RegisterType pointType, quint16 value, ByteOrder order, QVariant& outValue)
{
    QString result;
    value = toByteOrderValue(value, order);

    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            result = QString("<%1>").arg(value);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            quint8 lo, hi;
            breakUInt16(value, lo, hi, ByteOrder::LittleEndian);

            QList<quint8> bytes;
            bytes << hi << lo;

            QStringList chars;
            for(auto&& cb : bytes)
            {
                const QChar c(cb);
                if(std::isprint(cb) && cb != 13)
                    chars.append(c);
                else
                    chars.append(QString("\\x%1").arg(QString::number(cb, 16).toUpper(), 2, '0'));
            }
            result = QString("%1").arg(chars.join(' '));
        }
        break;
        default:
        break;
    }
    outValue = value;
    return result;
}

///
/// \brief formatFloatValue
/// \param pointType
/// \param value1
/// \param value2
/// \param order
/// \param flag
/// \param outValue
/// \return
///
inline QString formatFloatValue(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, ByteOrder order, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            outValue = value1;
            result = QString("<%1>").arg(value1);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const float value = makeFloat(value1, value2, order);
            outValue = value;
            result = QLocale().toString(value);
        }
        break;
        default:
            break;
    }
    return result;
}

///
/// \brief formatInt32Value
/// \param pointType
/// \param value1
/// \param value2
/// \param order
/// \param flag
/// \param outValue
/// \return
///
inline QString formatInt32Value(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, ByteOrder order, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            outValue = value1;
            result = QString("<%1>").arg(value1);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const qint32 value = makeInt32(value1, value2, order);
            outValue = value;
            result = QString("<%1>").arg(value, 10, 10, QLatin1Char(' '));
        }
        break;
        default:
            break;
    }
    return result;
}

///
/// \brief formatUInt32Value
/// \param pointType
/// \param value1
/// \param value2
/// \param order
/// \param flag
/// \param outValue
/// \return
///
inline QString formatUInt32Value(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, ByteOrder order, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            outValue = value1;
            result = QString("<%1>").arg(value1);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const quint32 value = makeUInt32(value1, value2, order);
            outValue = value;
            result = QString("<%1>").arg(value, 10, 10, QLatin1Char('0'));
        }
        break;
        default:
            break;
    }
    return result;
}

///
/// \brief formatDoubleValue
/// \param pointType
/// \param value1
/// \param value2
/// \param value3
/// \param value4
/// \param order
/// \param flag
/// \param outValue
/// \return
///
inline QString formatDoubleValue(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, quint16 value3, quint16 value4, ByteOrder order, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            outValue = value1;
            result = QString("<%1>").arg(value1);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const double value = makeDouble(value1, value2, value3, value4, order);
            outValue = value;
            result = QLocale().toString(value, 'g', 16);
        }
        break;
        default:
            break;
    }
    return result;
}

///
/// \brief formatInt64Value
/// \param pointType
/// \param value1
/// \param value2
/// \param value3
/// \param value4
/// \param order
/// \param flag
/// \param outValue
/// \return
///
inline QString formatInt64Value(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, quint16 value3, quint16 value4, ByteOrder order, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            outValue = value1;
            result = QString("<%1>").arg(value1);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const qint64 value = makeInt64(value1, value2, value3, value4, order);
            outValue = value;
            result = QString("<%1>").arg(value, 20, 10, QLatin1Char(' '));
        }
        break;
        default:
            break;
    }
    return result;
}

///
/// \brief formatUInt64Value
/// \param pointType
/// \param value1
/// \param value2
/// \param value3
/// \param value4
/// \param order
/// \param flag
/// \param outValue
/// \return
///
inline QString formatUInt64Value(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, quint16 value3, quint16 value4, ByteOrder order, bool flag, QVariant& outValue)
{
    QString result;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
        case QModbusDataUnit::DiscreteInputs:
            outValue = value1;
            result = QString("<%1>").arg(value1);
            break;
        case QModbusDataUnit::HoldingRegisters:
        case QModbusDataUnit::InputRegisters:
        {
            if(flag) break;

            const quint64 value = makeUInt64(value1, value2, value3, value4, order);
            outValue = value;
            result = QString("<%1>").arg(value, 20, 10, QLatin1Char('0'));
        }
        break;
        default:
            break;
    }
    return result;
}

///
/// \brief formatAddress
/// \param pointType
/// \param address
/// \param hexFormat
/// \return
///
inline QString formatAddress(QModbusDataUnit::RegisterType pointType, int address, bool hexFormat)
{
    QString prefix;
    switch(pointType)
    {
        case QModbusDataUnit::Coils:
            prefix = "0";
            break;
        case QModbusDataUnit::DiscreteInputs:
            prefix = "1";
            break;
        case QModbusDataUnit::HoldingRegisters:
            prefix = "4";
            break;
        case QModbusDataUnit::InputRegisters:
            prefix = "3";
            break;
        default:
            break;
    }

    return hexFormat ? QString("0x%1").arg(QString::number(address, 16).toUpper(), 4, '0') :
               prefix + QStringLiteral("%1").arg(address, 4, 10, QLatin1Char('0'));
}

#endif // FORMATUTILS_H
