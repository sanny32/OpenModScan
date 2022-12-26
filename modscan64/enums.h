#ifndef ENUMS_H
#define ENUMS_H

#include <QMetaType>

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
/// \brief The ConnectionType enum
///
enum class ConnectionType
{
    Tcp = 0,
    Serial
};
Q_DECLARE_METATYPE(ConnectionType);

#endif // ENUMS_H
