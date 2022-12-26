#ifndef ENUMS_H
#define ENUMS_H

///
/// \brief The DisplayMode enum
///
enum class DisplayMode
{
    Data = 0,
    Traffic
};

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

#endif // ENUMS_H
