#ifndef DISPLAYDEFINITION_H
#define DISPLAYDEFINITION_H

#include <QSettings>
#include <QModbusDataUnit>
#include <QXmlStreamWriter>
#include "modbuslimits.h"
#include "enums.h"

///
/// \brief The DisplayDefinition struct
///
struct DisplayDefinition
{
    QString FormName;
    quint32 ScanRate = 1000;
    quint8 DeviceId = 1;
    quint16 PointAddress = 1;
    QModbusDataUnit::RegisterType PointType = QModbusDataUnit::Coils;
    quint16 Length = 50;
    quint16 LogViewLimit = 30;
    bool ZeroBasedAddress = false;
    bool HexAddress = false;
    bool AutoscrollLog = false;
    AddressSpace AddrSpace = AddressSpace::Addr6Digits;
    quint16 DataViewColumnsDistance = 16;
    bool LeadingZeros = true;

    void normalize()
    {
        ScanRate = qBound(20U, ScanRate, 3600000U);
        DeviceId = qMax<quint8>(ModbusLimits::slaveRange().from(), DeviceId);
        PointAddress = qMax<quint16>(ModbusLimits::addressRange(ZeroBasedAddress).from(), PointAddress);
        PointType = qBound(QModbusDataUnit::DiscreteInputs, PointType, QModbusDataUnit::HoldingRegisters);
        Length = qBound<quint16>(ModbusLimits::lengthRange().from(), Length, ModbusLimits::lengthRange().to());
        LogViewLimit = qBound<quint16>(4, LogViewLimit, 1000);
        DataViewColumnsDistance = qBound<quint16>(1, DataViewColumnsDistance, 32);
    }
};
Q_DECLARE_METATYPE(DisplayDefinition)

inline QSettings& operator <<(QSettings& out, const DisplayDefinition& dd)
{
    out.setValue("DisplayDefinition/FormName",              dd.FormName);
    out.setValue("DisplayDefinition/ScanRate",              dd.ScanRate);
    out.setValue("DisplayDefinition/DeviceId",              dd.DeviceId);
    out.setValue("DisplayDefinition/PointAddress",          dd.PointAddress);
    out.setValue("DisplayDefinition/PointType",             dd.PointType);
    out.setValue("DisplayDefinition/Length",                dd.Length);
    out.setValue("DisplayDefinition/LogViewLimit",          dd.LogViewLimit);
    out.setValue("DisplayDefinition/DataViewColumnSpace",   dd.DataViewColumnsDistance);
    out.setValue("DisplayDefinition/LeadingZeros",          dd.LeadingZeros);
    out.setValue("DisplayDefinition/ZeroBasedAddress",      dd.ZeroBasedAddress);
    out.setValue("DisplayDefinition/HexAddress",            dd.HexAddress);
    out.setValue("DisplayDefinition/AutoscrollLog",         dd.AutoscrollLog);

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param dd
/// \return
///
inline QSettings& operator >>(QSettings& in, DisplayDefinition& dd)
{
    dd.FormName = in.value("DisplayDefinition/FormName").toString();
    dd.ScanRate = in.value("DisplayDefinition/ScanRate", 1000).toUInt();
    dd.DeviceId = in.value("DisplayDefinition/DeviceId", 1).toUInt();
    dd.PointAddress = in.value("DisplayDefinition/PointAddress", 1).toUInt();
    dd.PointType = (QModbusDataUnit::RegisterType)in.value("DisplayDefinition/PointType", 1).toUInt();
    dd.Length = in.value("DisplayDefinition/Length", 50).toUInt();
    dd.LogViewLimit = in.value("DisplayDefinition/LogViewLimit", 30).toUInt();
    dd.DataViewColumnsDistance = in.value("DisplayDefinition/DataViewColumnSpace", 16).toUInt();
    dd.LeadingZeros = in.value("DisplayDefinition/LeadingZeros", true).toBool();
    dd.ZeroBasedAddress = in.value("DisplayDefinition/ZeroBasedAddress").toBool();
    dd.HexAddress = in.value("DisplayDefinition/HexAddress").toBool();
    dd.AutoscrollLog = in.value("DisplayDefinition/AutoscrollLog").toBool();

    dd.normalize();
    return in;
}

///
/// \brief operator <<
/// \param xml
/// \param dd
/// \return
///
inline QXmlStreamWriter& operator <<(QXmlStreamWriter& xml, const DisplayDefinition& dd)
{
    xml.writeStartElement("DisplayDefinition");
    xml.writeAttribute("FormName", dd.FormName);
    xml.writeAttribute("ScanRate", QString::number(dd.ScanRate));
    xml.writeAttribute("DeviceId", QString::number(dd.DeviceId));
    xml.writeAttribute("PointType", enumToString<QModbusDataUnit::RegisterType>(dd.PointType));
    xml.writeAttribute("PointAddress", QString::number(dd.PointAddress));
    xml.writeAttribute("Length", QString::number(dd.Length));
    xml.writeAttribute("LogViewLimit", QString::number(dd.LogViewLimit));
    xml.writeAttribute("ZeroBasedAddress", boolToString(dd.ZeroBasedAddress));
    xml.writeAttribute("HexAddress", boolToString(dd.HexAddress));
    xml.writeAttribute("AutoscrollLog", boolToString(dd.AutoscrollLog));
    xml.writeAttribute("DataViewColumnsDistance", QString::number(dd.DataViewColumnsDistance));
    xml.writeAttribute("LeadingZeros", boolToString(dd.LeadingZeros));
    xml.writeEndElement();

    return xml;
}

///
/// \brief operator >>
/// \param xml
/// \param dd
/// \return
///
inline QXmlStreamReader& operator >>(QXmlStreamReader& xml, DisplayDefinition& dd)
{
    if (xml.isStartElement() && xml.name() == QLatin1String("DisplayDefinition")) {
        const QXmlStreamAttributes attributes = xml.attributes();

        if (attributes.hasAttribute("FormName")) {
            dd.FormName = attributes.value("FormName").toString();
        }

        if (attributes.hasAttribute("ScanRate")) {
            bool ok; const auto scanRate = attributes.value("ScanRate").toUInt(&ok);
            if (ok) dd.ScanRate = scanRate;
        }

        if (attributes.hasAttribute("DeviceId")) {
            bool ok; const quint8 deviceId = attributes.value("DeviceId").toUShort(&ok);
            if (ok) dd.DeviceId = deviceId;
        }

        if (attributes.hasAttribute("PointType")) {
            dd.PointType = enumFromString<QModbusDataUnit::RegisterType>(attributes.value("PointType").toString());
        }

        if (attributes.hasAttribute("PointAddress")) {
            bool ok; const quint16 pointAddress = attributes.value("PointAddress").toUShort(&ok);
            if (ok) dd.PointAddress = pointAddress;
        }

        if (attributes.hasAttribute("Length")) {
            bool ok; const quint16 length = attributes.value("Length").toUShort(&ok);
            if (ok) dd.Length = length;
        }

        if (attributes.hasAttribute("LogViewLimit")) {
            bool ok; const quint16 logViewLimit = attributes.value("LogViewLimit").toUShort(&ok);
            if (ok) dd.LogViewLimit = logViewLimit;
        }

        if (attributes.hasAttribute("ZeroBasedAddress")) {
            dd.ZeroBasedAddress = stringToBool(attributes.value("ZeroBasedAddress").toString());
        }

        if (attributes.hasAttribute("HexAddress")) {
            dd.HexAddress = stringToBool(attributes.value("HexAddress").toString());
        }

        if (attributes.hasAttribute("AutoscrollLog")) {
            dd.AutoscrollLog = stringToBool(attributes.value("AutoscrollLog").toString());
        }

        if (attributes.hasAttribute("DataViewColumnsDistance")) {
            bool ok; const quint16 distance = attributes.value("DataViewColumnsDistance").toUShort(&ok);
            if (ok) dd.DataViewColumnsDistance = distance;
        }

        if (attributes.hasAttribute("LeadingZeros")) {
            dd.LeadingZeros = stringToBool(attributes.value("LeadingZeros").toString());
        }

        xml.skipCurrentElement();

        dd.normalize();
    }

    return xml;
}

#endif // DISPLAYDEFINITION_H
