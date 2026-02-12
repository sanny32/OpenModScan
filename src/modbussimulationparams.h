#ifndef MODBUSSIMULATIONPARAMS_H
#define MODBUSSIMULATIONPARAMS_H

#include <QDataStream>
#include <QXmlStreamReader>
#include <QVersionNumber>
#include "qrange.h"
#include "enums.h"

///
/// \brief The RandomSimulationParams class
///
struct RandomSimulationParams
{
    QRange<double> Range = QRange<double>(0., 65535.);
};
Q_DECLARE_METATYPE(RandomSimulationParams)

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const RandomSimulationParams& params)
{
    out << params.Range;
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, RandomSimulationParams& params)
{
    in >> params.Range;
    return in;
}

///
/// \brief operator <<
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamWriter& operator <<(QXmlStreamWriter& xml, const RandomSimulationParams& params)
{
    xml.writeStartElement("RandomSimulationParams");
    xml << params.Range;
    xml.writeEndElement();

    return xml;
}

///
/// \brief operator >>
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamReader& operator >>(QXmlStreamReader& xml, RandomSimulationParams& params)
{
    if (xml.isStartElement() && xml.name() == QLatin1String("RandomSimulationParams")) {
        xml >> params.Range;
        xml.skipCurrentElement();
    }

    return xml;
}

///
/// \brief The IncrementSimulationParams class
///
struct IncrementSimulationParams
{
    double Step = 1.;
    QRange<double> Range = QRange<double>(0., 65535.);
};
Q_DECLARE_METATYPE(IncrementSimulationParams)

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const IncrementSimulationParams& params)
{
    out << params.Step;
    out << params.Range;
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, IncrementSimulationParams& params)
{
    in >> params.Step;
    in >> params.Range;
    return in;
}

///
/// \brief operator <<
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamWriter& operator <<(QXmlStreamWriter& xml, const IncrementSimulationParams& params)
{
    xml.writeStartElement("IncrementSimulationParams");
    xml.writeAttribute("Step", QString::number(params.Step));
    xml << params.Range;
    xml.writeEndElement();

    return xml;
}

///
/// \brief operator >>
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamReader& operator >>(QXmlStreamReader& xml, IncrementSimulationParams& params)
{
    if (xml.isStartElement() && xml.name() == QLatin1String("IncrementSimulationParams")) {
        const QXmlStreamAttributes attributes = xml.attributes();
        if (attributes.hasAttribute("Step")) {
            bool ok;
            double step = attributes.value("Step").toDouble(&ok);
            if (ok) {
                params.Step = step;
            }
        }

        xml >> params.Range;
        xml.skipCurrentElement();
    }

    return xml;
}

///
/// \brief The DecrementSimulationParams class
///
struct DecrementSimulationParams
{
    double Step = 1.;
    QRange<double> Range = QRange<double>(0., 65535.);
};
Q_DECLARE_METATYPE(DecrementSimulationParams)

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const DecrementSimulationParams& params)
{
    out << params.Step;
    out << params.Range;
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, DecrementSimulationParams& params)
{
    in >> params.Step;
    in >> params.Range;
    return in;
}

///
/// \brief operator <<
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamWriter& operator <<(QXmlStreamWriter& xml, const DecrementSimulationParams& params)
{
    xml.writeStartElement("DecrementSimulationParams");
    xml.writeAttribute("Step", QString::number(params.Step));
    xml << params.Range;
    xml.writeEndElement();

    return xml;
}

///
/// \brief operator >>
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamReader& operator >>(QXmlStreamReader& xml, DecrementSimulationParams& params)
{
    if (xml.isStartElement() && xml.name() == QLatin1String("DecrementSimulationParams")) {
        const QXmlStreamAttributes attributes = xml.attributes();
        if (attributes.hasAttribute("Step")) {
            bool ok;
            double step = attributes.value("Step").toDouble(&ok);
            if (ok) {
                params.Step = step;
            }
        }

        xml >> params.Range;
        xml.skipCurrentElement();
    }

    return xml;
}

///
/// \brief The ModbusSimulationParams class
///
struct ModbusSimulationParams
{
    ModbusSimulationParams(SimulationMode mode = SimulationMode::Off) {
        Mode = mode;
    }
    SimulationMode Mode = SimulationMode::Off;
    RandomSimulationParams RandomParams;
    IncrementSimulationParams IncrementParams;
    DecrementSimulationParams DecrementParams;
    quint32 Interval = 1000;
    DataDisplayMode DataMode = DataDisplayMode::Hex;
};
Q_DECLARE_METATYPE(ModbusSimulationParams)

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const ModbusSimulationParams& params)
{
    out << params.Mode;
    out << params.RandomParams;
    out << params.IncrementParams;
    out << params.DecrementParams;
    out << params.Interval;
    out << params.DataMode;

    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QDataStream& operator >>(QDataStream& in, ModbusSimulationParams& params)
{
    in >> params.Mode;
    in >> params.RandomParams;
    in >> params.IncrementParams;
    in >> params.DecrementParams;
    in >> params.Interval;

    if (in.device()->property("Form_Version").isValid() &&
        in.device()->property("Form_Version").value<QVersionNumber>() >= QVersionNumber(1, 10))
    {
        in >> params.DataMode;
    }

    return in;
}

///
/// \brief operator <<
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamWriter& operator <<(QXmlStreamWriter& xml, const ModbusSimulationParams& params)
{
    xml.writeStartElement("ModbusSimulationParams");
    xml.writeAttribute("Mode", enumToString<SimulationMode>(params.Mode));
    xml.writeAttribute("Interval", QString::number(params.Interval));
    xml.writeAttribute("DataDisplayMode", enumToString<DataDisplayMode>(params.DataMode));

    switch(params.Mode) {
    case SimulationMode::Random:
        xml << params.RandomParams;     break;
    case SimulationMode::Increment:
        xml << params.IncrementParams;  break;
    case SimulationMode::Decrement:
        xml << params.DecrementParams;  break;
    default: break;
    }

    xml.writeEndElement();

    return xml;
}

///
/// \brief operator >>
/// \param xml
/// \param params
/// \return
///
inline QXmlStreamReader& operator >>(QXmlStreamReader& xml, ModbusSimulationParams& params)
{
    if (xml.isStartElement() && xml.name() == QLatin1String("ModbusSimulationParams")) {
        const QXmlStreamAttributes attributes = xml.attributes();

        if (attributes.hasAttribute("Mode")) {
            params.Mode = enumFromString<SimulationMode>(attributes.value("Mode").toString(), SimulationMode::Off);
        }

        if (attributes.hasAttribute("Interval")) {
            bool ok; quint32 interval = attributes.value("Interval").toUInt(&ok);
            if (ok) {
                params.Interval = interval;
            }
        }

        if (attributes.hasAttribute("DataDisplayMode")) {
            params.DataMode = enumFromString<DataDisplayMode>(attributes.value("DataDisplayMode").toString(), DataDisplayMode::Hex);
        }

        while (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("RandomSimulationParams") &&
                params.Mode == SimulationMode::Random) {
                xml >> params.RandomParams;
            } else if (xml.name() == QLatin1String("IncrementSimulationParams") &&
                       params.Mode == SimulationMode::Increment) {
                xml >> params.IncrementParams;
            } else if (xml.name() == QLatin1String("DecrementSimulationParams") &&
                       params.Mode == SimulationMode::Decrement) {
                xml >> params.DecrementParams;
            } else {
                xml.skipCurrentElement();
            }
        }
    }

    return xml;
}

#endif // MODBUSSIMULATIONPARAMS_H
