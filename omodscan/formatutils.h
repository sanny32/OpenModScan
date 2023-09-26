#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include <QString>
#include <QLocale>
#include <QModbusDataUnit>
#include "enums.h"
#include "numericutils.h"
#include "byteorderutils.h"

///
/// \brief formatByteValue
/// \param mode
/// \param c
/// \return
///
inline QString formatByteValue(DataDisplayMode mode, uchar c) {
    switch(mode)
    {
        case DataDisplayMode::Decimal:
        case DataDisplayMode::Integer:
            return QString("%1").arg(QString::number(c), 3, '0');

        default:
            return QString("%1").arg(QString::number(c, 16).toUpper(), 2, '0');
    }
}

///
/// \brief formatByteArray
/// \param mode
/// \param ar
/// \return
///
inline QString formatByteArray(DataDisplayMode mode, const QByteArray& ar)
{
    QStringList values;
    for(auto i = 0; i < ar.size(); i++)
        values += formatByteValue(mode, ar[i]);

    return values.join(" ");
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
/// \brief formatDecimalValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
inline QString formatDecimalValue(QModbusDataUnit::RegisterType pointType, quint16 value, ByteOrder order, QVariant& outValue)
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
/// \brief formatIntegerValue
/// \param pointType
/// \param value
/// \param outValue
/// \return
///
inline QString formatIntegerValue(QModbusDataUnit::RegisterType pointType, qint16 value, ByteOrder order, QVariant& outValue)
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
        result = QStringLiteral("<%1H>").arg(value, 4, 16, QLatin1Char('0'));
        break;
    default:
        break;
    }
    outValue = value;
    return result.toUpper();
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
/// \brief formatLongValue
/// \param pointType
/// \param value1
/// \param value2
/// \param order
/// \param flag
/// \param outValue
/// \return
///
inline QString formatLongValue(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, ByteOrder order, bool flag, QVariant& outValue)
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

        const qint32 value = makeLong(value1, value2, order);
        outValue = value;
        result = result = QString("<%1>").arg(value, 10, 10, QLatin1Char(' '));
    }
    break;
    default:
        break;
    }
    return result;
}

///
/// \brief formatUnsignedLongValue
/// \param pointType
/// \param value1
/// \param value2
/// \param order
/// \param flag
/// \param outValue
/// \return
///
inline QString formatUnsignedLongValue(QModbusDataUnit::RegisterType pointType, quint16 value1, quint16 value2, ByteOrder order, bool flag, QVariant& outValue)
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

        const quint32 value = makeULong(value1, value2, order);
        outValue = value;
        result = result = QString("<%1>").arg(value, 10, 10, QLatin1Char('0'));
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

    return hexFormat ? QStringLiteral("%1H").arg(address, 4, 16, QLatin1Char('0')) :
               prefix + QStringLiteral("%1").arg(address, 4, 10, QLatin1Char('0'));
}

#endif // FORMATUTILS_H
