#ifndef ENUMS_H
#define ENUMS_H

#include <QMetaType>
#include <QSettings>

template<typename Enum>
struct EnumStrings {
    static const QMap<Enum, QString>& mapping() {
        static const QMap<Enum, QString> map;
        return map;
    }
};

#define DECLARE_ENUM_STRINGS(EnumType, ...) \
template<> \
    struct EnumStrings<EnumType> { \
        static const QMap<EnumType, QString>& mapping() { \
            static const QMap<EnumType, QString> map = { __VA_ARGS__ }; \
            return map; \
    } \
};

///
/// \brief The AddressBase enum
///
enum class AddressBase
{
    Base0 = 0,
    Base1
};
Q_DECLARE_METATYPE(AddressBase);
DECLARE_ENUM_STRINGS(AddressBase,
                     {   AddressBase::Base0, "Base0" },
                     {   AddressBase::Base1, "Base1" }
)

///
/// \brief operator <<
/// \param out
/// \param params
/// \return
///
inline QSettings& operator <<(QSettings& out, const AddressBase& base)
{
    out.setValue("AddressBase", (uint)base);
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param params
/// \return
///
inline QSettings& operator >>(QSettings& in, AddressBase& base)
{
    base = (AddressBase)in.value("AddressBase").toUInt();
    return in;
}

///
/// \brief The AddressSpace enum
///
enum class AddressSpace
{
    Addr6Digits = 0,
    Addr5Digits
};
Q_DECLARE_METATYPE(AddressSpace)
DECLARE_ENUM_STRINGS(AddressSpace,
                     {   AddressSpace::Addr6Digits, "6-Digits"    },
                     {   AddressSpace::Addr5Digits, "5-Digits"    }
)

///
/// \brief The DisplayMode enum
///
enum class DisplayMode
{
    Data = 0,
    Traffic
};
Q_DECLARE_METATYPE(DisplayMode);
DECLARE_ENUM_STRINGS(DisplayMode,
                     {   DisplayMode::Data,      "Data"      },
                     {   DisplayMode::Traffic,   "Traffic"   }
)

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
    UInt16,
    Int16,
    Hex,
    FloatingPt,     // MSRF
    SwappedFP,      // LSRF
    DblFloat,       // MSRF
    SwappedDbl,     // LSRF
    Int32,          // MSRF
    SwappedInt32,   // LSRF
    UInt32,         // MSRF
    SwappedUInt32,  // LSRF
    Int64,          // MSRF
    SwappedInt64,   // LSRF
    UInt64,         // MSRF
    SwappedUInt64,  // LSRF
    Ansi
};
Q_DECLARE_METATYPE(DataDisplayMode);
DECLARE_ENUM_STRINGS(DataDisplayMode,
                     {   DataDisplayMode::Binary,          "Binary"        },
                     {   DataDisplayMode::UInt16,          "UInt16"        },
                     {   DataDisplayMode::Int16,           "Int16"         },
                     {   DataDisplayMode::Hex,             "Hex"           },
                     {   DataDisplayMode::FloatingPt,      "FloatingPt"    },
                     {   DataDisplayMode::SwappedFP,       "SwappedFP"     },
                     {   DataDisplayMode::DblFloat,        "DblFloat"      },
                     {   DataDisplayMode::SwappedDbl,      "SwappedDbl"    },
                     {   DataDisplayMode::Int32,           "Int32"         },
                     {   DataDisplayMode::SwappedInt32,    "SwappedInt32"  },
                     {   DataDisplayMode::UInt32,          "UInt32"        },
                     {   DataDisplayMode::SwappedUInt32,   "SwappedUInt32" },
                     {   DataDisplayMode::Int64,           "Int64"         },
                     {   DataDisplayMode::SwappedInt64,    "SwappedInt64"  },
                     {   DataDisplayMode::UInt64,          "UInt64"        },
                     {   DataDisplayMode::SwappedUInt64,   "SwappedUInt64" },
                     {   DataDisplayMode::Ansi,            "Ansi"          },
)

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
/// \brief The ByteOrder enum
///
enum class ByteOrder
{
    Direct = 0,
    Swapped
};
Q_DECLARE_METATYPE(ByteOrder);
DECLARE_ENUM_STRINGS(ByteOrder,
                     {   ByteOrder::Direct,    "Direct"    },
                     {   ByteOrder::Swapped,   "Swapped"   }
)

enum class WordOrder {
    MSRF,  // Most Significant Register First (hi, lo)
    LSRF   // Least Significant Register First (lo, hi)
};
Q_DECLARE_METATYPE(WordOrder);
DECLARE_ENUM_STRINGS(WordOrder,
                     {   WordOrder::MSRF,   "MSRF"   },
                     {   WordOrder::LSRF,   "LSRF"   }
                     )

///
/// \brief operator <<
/// \param out
/// \param order
/// \return
///
inline QSettings& operator <<(QSettings& out, const ByteOrder& order)
{
    out.setValue("ByteOrder", (uint)order);
    return out;
}

///
/// \brief operator >>
/// \param in
/// \param order
/// \return
///
inline QSettings& operator >>(QSettings& in, ByteOrder& order)
{
    order = (ByteOrder)in.value("ByteOrder").toUInt();
    return in;
}

///
/// \brief The ConnectionType enum
///
enum class ConnectionType
{
    Tcp = 0,
    Serial,
};
Q_DECLARE_METATYPE(ConnectionType);
DECLARE_ENUM_STRINGS(ConnectionType,
                     {   ConnectionType::Tcp,      "Tcp"    },
                     {   ConnectionType::Serial,   "Serial" }
)


///
/// \brief The TransmissionMode enum
///
enum class TransmissionMode
{
    ASCII = 0,
    RTU,
    IP
};
Q_DECLARE_METATYPE(TransmissionMode);
DECLARE_ENUM_STRINGS(TransmissionMode,
                     {   TransmissionMode::ASCII, "ASCII"  },
                     {   TransmissionMode::RTU,   "RTU"    },
                     {   TransmissionMode::IP,    "IP"    }
)


///
/// \brief The CaptureMode enum
///
enum class CaptureMode
{
    Off = 0,
    TextCapture
};
Q_DECLARE_METATYPE(CaptureMode);
DECLARE_ENUM_STRINGS(CaptureMode,
                     {   CaptureMode::Off,         "Off"           },
                     {   CaptureMode::TextCapture, "TextCapture"   }
)

///
/// \brief The SimulationMode enum
///
enum class SimulationMode
{
    Disabled = 0,
    Off,
    Random,
    Increment,
    Decrement,
    Toggle
};
Q_DECLARE_METATYPE(SimulationMode);
DECLARE_ENUM_STRINGS(SimulationMode,
                     {   SimulationMode::Disabled,    "Disabled"    },
                     {   SimulationMode::Off,         "Off"         },
                     {   SimulationMode::Random,      "Random"      },
                     {   SimulationMode::Increment,   "Increment"   },
                     {   SimulationMode::Decrement,   "Decrement"   },
                     {   SimulationMode::Toggle,      "Toggle"      }
)

///
/// \brief The PollState enum
///
enum class PollState {
    Off,
    Running,
    Paused
};
Q_DECLARE_METATYPE(PollState);
DECLARE_ENUM_STRINGS(PollState,
                     {   PollState::Off,        "Off"       },
                     {   PollState::Running,    "Running"   },
                     {   PollState::Paused,     "Paused"    }
)

///
/// \brief QString
///
enum class SerializationFormat {
    Binary = 0,
    Xml
};
Q_DECLARE_METATYPE(SerializationFormat)
DECLARE_ENUM_STRINGS(SerializationFormat,
                     {   SerializationFormat::Binary,    "Binary"    },
                     {   SerializationFormat::Xml,       "Xml"       },
)

///
/// \brief boolToString
/// \param value
/// \return
///
inline QString boolToString(bool value)
{
    return value ? "true" : "false";
}

///
/// \brief stringToBool
/// \param str
/// \return
///
inline bool stringToBool(const QString& str)
{
    const QString lower = str.toLower();
    return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
}

///
/// \brief enumToString
/// \param value
/// \return
///
template<typename Enum>
inline QString enumToString(Enum value) {
    const auto& map = EnumStrings<Enum>::mapping();
    if (auto it = map.find(value); it != map.end())
        return it.value();
    return QString::number(static_cast<int>(value));
}

///
/// \brief enumFromString
/// \param str
/// \param defaultValue
/// \return
///
template<typename Enum>
inline Enum enumFromString(const QString& str, Enum defaultValue = static_cast<Enum>(0)) {
    const auto& map = EnumStrings<Enum>::mapping();
    for (auto it = map.begin(); it != map.end(); ++it) {
        if (it.value().compare(str, Qt::CaseInsensitive) == 0)
            return it.key();
    }
    bool ok;
    int val = str.toInt(&ok);
    if (ok)
        return static_cast<Enum>(val);
    return defaultValue;
}

#endif // ENUMS_H
