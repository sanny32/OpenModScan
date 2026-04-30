#ifndef MODBUSWRITEPARAMS_H
#define MODBUSWRITEPARAMS_H

#include <QSettings>
#include <QVariant>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "enums.h"

class ModbusClient;

///
/// \brief The PulseParams class
///
struct PulseParams
{
    bool Enabled = false;
    int Duration = 500;

    enum RestoreMode {
        Previous = 0,
        Zero
    } Restore = Previous;
};

Q_DECLARE_METATYPE(PulseParams::RestoreMode);
DECLARE_ENUM_STRINGS(PulseParams::RestoreMode,
                     {   PulseParams::Previous, "Previous" },
                     {   PulseParams::Zero,     "Zero"     }
)

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QSettings& operator <<(QSettings& out, const PulseParams& params)
{
    out.beginGroup("PulseParams");
    out.setValue("Duration", params.Duration);
    out.setValue("Restore", (uint)params.Restore);
    out.endGroup();

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QSettings& operator >>(QSettings& in, PulseParams& params)
{
    in.beginGroup("PulseParams");
    params.Duration = in.value("Duration", params.Duration).toInt();
    params.Restore = (PulseParams::RestoreMode)in.value("Restore", (uint)params.Restore).toUInt();
    in.endGroup();

    return in;
}

///
/// \brief operator <<
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamWriter& operator <<(QXmlStreamWriter& xml, const PulseParams& params)
{
    xml.writeStartElement("PulseParams");
    xml.writeAttribute("Duration", QString::number(params.Duration));
    xml.writeAttribute("Restore", enumToString<PulseParams::RestoreMode>(params.Restore));
    xml.writeEndElement();

    return xml;
}

///
/// \brief operator >>
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamReader& operator >>(QXmlStreamReader& xml, PulseParams& params)
{
    if (xml.isStartElement() && xml.name() == QLatin1String("PulseParams")) {
        const QXmlStreamAttributes attributes = xml.attributes();

        if (attributes.hasAttribute("Duration")) {
            bool ok;
            const int duration = attributes.value("Duration").toInt(&ok);
            if (ok && duration >= 0) params.Duration = duration;
        }

        if (attributes.hasAttribute("Restore")) {
            params.Restore = enumFromString<PulseParams::RestoreMode>(
                attributes.value("Restore").toString(), params.Restore);
        }

        xml.skipCurrentElement();
    }

    return xml;
}

///
/// \brief The ModbusWriteParams class
///
struct ModbusWriteParams
{
    quint32 DeviceId;
    quint32 Address;
    QVariant Value;
    DataDisplayMode DisplayMode;
    AddressSpace AddrSpace = AddressSpace::Addr6Digits;
    ByteOrder Order;
    QString Codepage;
    PulseParams PusleParams;
    bool ZeroBasedAddress;
    bool LeadingZeros = false;
    bool ForceModbus15And16Func = false;
    ModbusClient* Client = nullptr;
};

///
/// \brief The ModbusMaskWriteParams class
///
struct ModbusMaskWriteParams
{
    quint32 DeviceId;
    quint32 Address;
    quint16 AndMask;
    quint16 OrMask;
    bool ZeroBasedAddress;
    bool LeadingZeros = false;
};

#endif // MODBUSWRITEPARAMS_H
