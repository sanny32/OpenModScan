#ifndef ENUMS_H
#define ENUMS_H

#include <QMetaType>
#include <QSettings>

///
/// \brief The DisplayMode enum
///
enum class DisplayMode
{
    Data = 0,
    Traffic
};
Q_DECLARE_METATYPE(DisplayMode);

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QSettings& operator <<(QSettings& out, const DisplayMode& mode)
{
    out.setValue("DisplayMode", (uint)mode);
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QSettings& operator >>(QSettings& in, DisplayMode& mode)
{
    mode = (DisplayMode)in.value("DisplayMode").toUInt();
    return in;
}

///
/// \brief The DataDisplayMode enum
///
enum class DataDisplayMode
{
    Binary = 0,
    Decimal,
    Integer,
    Hex,
    FloatingPt,
    SwappedFP,
    DblFloat,
    SwappedDbl
};
Q_DECLARE_METATYPE(DataDisplayMode);

///
/// \brief operator <<
/// \param out
/// \param mode
/// \return
///
inline QSettings& operator <<(QSettings& out, const DataDisplayMode& mode)
{
    out.setValue("DataDisplayMode", (uint)mode);
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param mode
/// \return
///
inline QSettings& operator >>(QSettings& in, DataDisplayMode& mode)
{
    mode = (DataDisplayMode)in.value("DataDisplayMode").toUInt();
    return in;
}

///
/// \brief The ConnectionType enum
///
enum class ConnectionType
{
    Tcp = 0,
    Serial
};
Q_DECLARE_METATYPE(ConnectionType);

///
/// \brief The TransmissionMode enum
///
enum class TransmissionMode
{
    ASCII = 0,
    RTU
};
Q_DECLARE_METATYPE(TransmissionMode);

///
/// \brief The CaptureMode enum
///
enum class CaptureMode
{
    Off = 0,
    TextCapture
};
Q_DECLARE_METATYPE(CaptureMode);

#endif // ENUMS_H
