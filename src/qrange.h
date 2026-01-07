#ifndef QRANGE_H
#define QRANGE_H

#include <limits>
#include <QDataStream>
#include <QXmlStreamWriter>

///
/// \brief The QRange class
///
template<class T>
class QRange
{

    friend QDataStream& operator <<(QDataStream& out, const QRange<double>& range);
    friend QDataStream& operator >>(QDataStream& in, QRange<double>& range);

public:
    ///
    /// \brief QRange
    ///
    explicit QRange()
    {
        _from = std::numeric_limits<T>::min();
        _to = std::numeric_limits<T>::max();
    }

    ///
    /// \brief QRange
    /// \param from
    /// \param to
    ///
    QRange(T from, T to)
    {
        _from = from < to ? from : to;
        _to = from < to ? to : from;
    }

    ///
    /// \brief from
    /// \return
    ///
    T from() const { return _from;}

    ///
    /// \brief to
    /// \return
    ///
    T to() const { return _to; };

    ///
    /// \brief contains
    /// \param num
    /// \return
    ///
    bool contains(T num) const
    {
        return num >= _from && num <= _to;
    }

private:
    T _from;
    T _to;
};

///
/// \brief operator <<
/// \param out
/// \param range
/// \return
///
inline QDataStream& operator <<(QDataStream& out, const QRange<double>& range)
{
    out << range.from();
    out << range.to();
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param range
/// \return
///
inline QDataStream& operator >>(QDataStream& in, QRange<double>& range)
{
    in >> range._from;
    in >> range._to;
    return in;
}

///
/// \brief operator <<
/// \param xml
/// \param range
/// \return
///
inline QXmlStreamWriter& operator <<(QXmlStreamWriter& xml, const QRange<double>& range)
{
    xml.writeStartElement("Range");
    xml.writeAttribute("From", QString::number(range.from()));
    xml.writeAttribute("To", QString::number(range.to()));
    xml.writeEndElement();

    return xml;
}

///
/// \brief operator >>
/// \param xml
/// \param range
/// \return
///
inline QXmlStreamReader& operator >>(QXmlStreamReader& xml, QRange<double>& range)
{
    if (xml.isStartElement() && xml.name() == QLatin1String("Range")) {
        const QXmlStreamAttributes attributes = xml.attributes();

        bool okFrom, okTo;
        const double from = attributes.value("From").toDouble(&okFrom);
        const double to = attributes.value("To").toDouble(&okTo);

        if (okFrom && okTo) {
            range = QRange<double>(from, to);
        } else {
            range = QRange<double>();
        }

        xml.skipCurrentElement();
    }

    return xml;
}

#endif // QRANGE_H
