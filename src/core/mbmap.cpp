#include <QDir>
#include <QFile>
#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <algorithm>
#include <tuple>
#include "modbuslimits.h"
#include "mbmap.h"

namespace {

///
/// \brief Addresses left empty between two registers before they go to
/// separate windows
///
constexpr int MaxGap = 10;

///
/// \brief An integer written as a JSON number or as a string holding a
/// decimal or 0x hex integer (SPEC.md §2.1)
///
bool toInteger(const QJsonValue& v, qint64& out)
{
    if(v.isDouble()) {
        const double d = v.toDouble();
        if(d != static_cast<double>(static_cast<qint64>(d)))
            return false;
        out = static_cast<qint64>(d);
        return true;
    }
    if(v.isString()) {
        const QString s = v.toString();
        bool ok = false;
        out = s.startsWith("0x", Qt::CaseInsensitive) ? s.mid(2).toLongLong(&ok, 16) : s.toLongLong(&ok, 10);
        return ok;
    }
    return false;
}

QModbusDataUnit::RegisterType toArea(const QString& area)
{
    if(area == QLatin1String("coil"))     return QModbusDataUnit::Coils;
    if(area == QLatin1String("discrete")) return QModbusDataUnit::DiscreteInputs;
    if(area == QLatin1String("input"))    return QModbusDataUnit::InputRegisters;
    if(area == QLatin1String("holding"))  return QModbusDataUnit::HoldingRegisters;
    return QModbusDataUnit::Invalid;
}

///
/// \brief Registers a type takes (strings: their length)
///
int registerCount(const MbMap::Register& r)
{
    if(r.Type == QLatin1String("string")) return r.Length;
    if(r.Type.endsWith(QLatin1String("64"))) return 4;
    if(r.Type.endsWith(QLatin1String("32"))) return 2;
    return 1;
}

///
/// \brief How Open ModScan shows a register: one value per 1, 2 or 4
/// registers from the start of the window
///
DataDisplayMode displayMode(const MbMap::Register& r)
{
    const bool swap = r.WordSwapped;
    const QString& t = r.Type;
    if(t == QLatin1String("bool") || r.Bits > 0) return DataDisplayMode::Binary;
    if(t == QLatin1String("int16"))   return DataDisplayMode::Int16;
    if(t == QLatin1String("uint16"))  return DataDisplayMode::UInt16;
    if(t == QLatin1String("int32"))   return swap ? DataDisplayMode::SwappedInt32 : DataDisplayMode::Int32;
    if(t == QLatin1String("uint32"))  return swap ? DataDisplayMode::SwappedUInt32 : DataDisplayMode::UInt32;
    if(t == QLatin1String("float32")) return swap ? DataDisplayMode::SwappedFP : DataDisplayMode::FloatingPt;
    if(t == QLatin1String("int64"))   return swap ? DataDisplayMode::SwappedInt64 : DataDisplayMode::Int64;
    if(t == QLatin1String("uint64"))  return swap ? DataDisplayMode::SwappedUInt64 : DataDisplayMode::UInt64;
    if(t == QLatin1String("float64")) return swap ? DataDisplayMode::SwappedDbl : DataDisplayMode::DblFloat;
    return DataDisplayMode::Ansi; // string
}

int modeWidth(DataDisplayMode mode)
{
    switch(mode)
    {
        case DataDisplayMode::Int32:
        case DataDisplayMode::SwappedInt32:
        case DataDisplayMode::UInt32:
        case DataDisplayMode::SwappedUInt32:
        case DataDisplayMode::FloatingPt:
        case DataDisplayMode::SwappedFP:
            return 2;
        case DataDisplayMode::Int64:
        case DataDisplayMode::SwappedInt64:
        case DataDisplayMode::UInt64:
        case DataDisplayMode::SwappedUInt64:
        case DataDisplayMode::DblFloat:
        case DataDisplayMode::SwappedDbl:
            return 4;
        default:
            return 1;
    }
}

///
/// \brief The text shown next to a register: its name, the bits it takes,
/// its unit and the scale to apply to the raw value
///
QString describe(const MbMap::Register& r)
{
    QString text;
    if(r.Bits > 1)
        text = QString("bits %1-%2: ").arg(r.Bit).arg(r.Bit + r.Bits - 1);
    else if(r.Bit >= 0 && r.Area != QModbusDataUnit::Coils && r.Area != QModbusDataUnit::DiscreteInputs)
        text = QString("bit %1: ").arg(r.Bit);

    text += r.Name;
    if(!r.Unit.isEmpty())
        text += QString(" [%1]").arg(r.Unit);
    if(r.Scale != 1)
        text += QString(" x%1").arg(r.Scale);
    if(r.Offset != 0)
        text += QString(" %1%2").arg(r.Offset > 0 ? "+" : "").arg(r.Offset);
    return text;
}

} // namespace

///
/// \brief Tells a device map from other files by its name
/// \param filename Name or path of a file
/// \return true when the name ends in .mbmap.json, in any case
///
bool MbMap::isMapFile(const QString& filename)
{
    return filename.endsWith(".mbmap.json", Qt::CaseInsensitive);
}

///
/// \brief Reads a map from a .mbmap.json file
/// \param filename Path of the file
/// \param error Set to the reason when the file cannot be read or is not a
/// map Open ModScan can read, cleared otherwise; may be null
/// \return The map; an empty map on error
///
MbMap MbMap::load(const QString& filename, QString* error)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly)) {
        if(error) *error = file.errorString();
        return MbMap();
    }
    return parse(file.readAll(), error);
}

///
/// \brief Reads a map from the content of a .mbmap.json file
/// \param data The file's content, in UTF-8, with or without a byte order mark
/// \param error Set to the reason when the content is not a map Open ModScan
/// can read, cleared otherwise; may be null
/// \return The map, without the registers that have errors; an empty map on error
///
MbMap MbMap::parse(const QByteArray& data, QString* error)
{
    MbMap map;
    auto fail = [&](const QString& message) {
        if(error) *error = message;
        return MbMap();
    };

    QByteArray content = data;
    if(content.startsWith("\xEF\xBB\xBF"))
        content.remove(0, 3);

    QJsonParseError parseError;
    const auto doc = QJsonDocument::fromJson(content, &parseError);
    if(parseError.error != QJsonParseError::NoError || !doc.isObject())
        return fail(QObject::tr("Not a valid JSON file: %1").arg(parseError.errorString()));

    const auto root = doc.object();
    qint64 version = 0;
    if(!toInteger(root.value("mbmap"), version))
        return fail(QObject::tr("Not a .mbmap file: the \"mbmap\" version is missing"));
    if(version != 2)
        return fail(QObject::tr("Unsupported .mbmap version %1 (version 2 is supported)").arg(version));

    map.Vendor = root.value("vendor").toString();
    map.Model = root.value("model").toString();
    map.MapDescription = root.value("description").toString();
    if(map.Vendor.isEmpty() || map.Model.isEmpty() || !root.value("registers").isArray())
        return fail(QObject::tr("The map has no \"vendor\", \"model\" or \"registers\""));

    qint64 n = 0;
    if(root.contains("unit_id") && toInteger(root.value("unit_id"), n) && n >= 0 && n <= 255)
        map.UnitId = static_cast<quint8>(n);
    if(root.contains("address_base") && toInteger(root.value("address_base"), n) && (n == 0 || n == 1))
        map.AddressBase = static_cast<int>(n);

    // Largest read the device accepts; out of range values keep the default
    if(root.contains("max_read_registers") && toInteger(root.value("max_read_registers"), n) && n >= 1 && n <= 125)
        map.MaxReadRegisters = static_cast<int>(n);
    if(root.contains("max_read_bits") && toInteger(root.value("max_read_bits"), n) && n >= 1 && n <= 2000)
        map.MaxReadBits = static_cast<int>(n);

    const bool fileByteSwap = root.value("byte_order").toString() == QLatin1String("little");
    const bool fileWordSwap = root.value("word_order").toString() == QLatin1String("little");

    static const QSet<QString> types = {
        "bool", "int16", "uint16", "int32", "uint32", "float32",
        "int64", "uint64", "float64", "string"
    };

    for(auto&& item : root.value("registers").toArray())
    {
        const auto obj = item.toObject();
        Register r;
        r.Name = obj.value("name").toString();
        r.Group = obj.value("group").toString();
        r.Unit = obj.value("unit").toString();
        r.Description = obj.value("description").toString();
        r.Area = toArea(obj.value("area").toString());
        r.Type = obj.value("type").toString();

        qint64 address = 0;
        if(r.Name.isEmpty() || r.Area == QModbusDataUnit::Invalid || !types.contains(r.Type)
           || !toInteger(obj.value("address"), address))
            continue;

        const bool bitArea = r.Area == QModbusDataUnit::Coils || r.Area == QModbusDataUnit::DiscreteInputs;
        if(bitArea && r.Type != QLatin1String("bool"))
            continue;

        address -= map.AddressBase;
        if(address < 0 || address > 65535)
            continue;
        r.Address = static_cast<quint16>(address);
        r.Readable = obj.value("access").toString() != QLatin1String("W");

        // A write-only register that cannot be shown still keeps reads off
        // its addresses
        auto blockAddresses = [&](qint64 count) {
            if(!r.Readable)
                map.WriteOnlyRanges.append({ r.Area, r.Address, static_cast<int>(qBound<qint64>(1, count, 65536 - address)) });
        };

        if(r.Type == QLatin1String("string")) {
            const bool hasLength = toInteger(obj.value("length"), n) && n >= 1;
            if(!hasLength || n > 125) {
                blockAddresses(hasLength ? n : 1);
                continue;
            }
            r.Length = static_cast<quint16>(n);
        }
        if(obj.contains("bit") && toInteger(obj.value("bit"), n) && n >= 0 && n <= 15)
            r.Bit = static_cast<int>(n);
        if(!bitArea && r.Type == QLatin1String("bool") && r.Bit < 0) {
            blockAddresses(1);
            continue;
        }
        if(r.Type == QLatin1String("uint16") && r.Bit >= 0 && obj.contains("bits")
           && toInteger(obj.value("bits"), n) && n >= 1 && r.Bit + n <= 16)
            r.Bits = static_cast<int>(n);

        if(obj.value("scale").isDouble()) r.Scale = obj.value("scale").toDouble();
        if(obj.value("offset").isDouble()) r.Offset = obj.value("offset").toDouble();

        r.ByteSwapped = obj.contains("byte_order") ? obj.value("byte_order").toString() == QLatin1String("little") : fileByteSwap;
        r.WordSwapped = obj.contains("word_order") ? obj.value("word_order").toString() == QLatin1String("little") : fileWordSwap;

        if(static_cast<int>(r.Address) + registerCount(r) > 65536) {
            blockAddresses(registerCount(r));
            continue;
        }

        map.Registers.append(r);
    }

    if(error) error->clear();
    return map;
}

///
/// \brief The map's name for window titles and dialogs
/// \return The vendor and model, e.g. "Example Corp EM-100"
///
QString MbMap::title() const
{
    return QString("%1 %2").arg(Vendor, Model);
}

///
/// \brief Lays the readable registers out in Open ModScan windows
/// \return One view per window, by area and then address. A window holds
/// registers of one area, display mode and byte order that are close
/// together and line up with its 2- or 4-register values. It is never
/// longer than Open ModScan's limit or the map's max_read_registers /
/// max_read_bits, and never covers an address that is only written.
/// Registers longer than those limits on their own are left out.
///
QList<MbMap::View> MbMap::views() const
{
    struct Key {
        QModbusDataUnit::RegisterType Area;
        DataDisplayMode Mode;
        ByteOrder Order;
        bool operator<(const Key& o) const {
            return std::tie(Area, Mode, Order) < std::tie(o.Area, o.Mode, o.Order);
        }
    };

    // Addresses that are only written: no window may read them, since a
    // device can refuse the whole request for one of them
    QMap<QModbusDataUnit::RegisterType, QSet<int>> writeOnly;
    for(auto&& range : WriteOnlyRanges) {
        for(int a = range.Address; a < range.Address + range.Count; a++)
            writeOnly[range.Area].insert(a);
    }
    for(auto&& r : Registers) {
        if(!r.Readable)
            for(int a = r.Address; a < r.Address + registerCount(r); a++)
                writeOnly[r.Area].insert(a);
    }
    for(auto&& r : Registers) {
        if(r.Readable)
            for(int a = r.Address; a < r.Address + registerCount(r); a++)
                writeOnly[r.Area].remove(a);
    }

    // Open ModScan's limit, and the device's for the area
    auto maxLength = [this](QModbusDataUnit::RegisterType area) {
        const bool bitArea = area == QModbusDataUnit::Coils || area == QModbusDataUnit::DiscreteInputs;
        return qMin(ModbusLimits::lengthRange().to(), bitArea ? MaxReadBits : MaxReadRegisters);
    };

    QMap<Key, QList<Register>> groups;
    for(auto&& r : Registers) {
        if(!r.Readable || registerCount(r) > maxLength(r.Area))
            continue;
        const auto mode = displayMode(r);
        const bool bitArea = r.Area == QModbusDataUnit::Coils || r.Area == QModbusDataUnit::DiscreteInputs;
        // Byte order does not apply to bool (SPEC.md), nor to coils and inputs
        const bool isBool = r.Type == QLatin1String("bool");
        const auto order = (r.ByteSwapped && !bitArea && !isBool) ? ByteOrder::Swapped : ByteOrder::Direct;
        groups[{ r.Area, mode, order }].append(r);
    }

    QList<View> views;

    for(auto it = groups.begin(); it != groups.end(); ++it)
    {
        auto regs = it.value();
        std::stable_sort(regs.begin(), regs.end(), [](const Register& a, const Register& b) {
            return a.Address < b.Address;
        });

        const int width = modeWidth(it.key().Mode);
        const int limit = maxLength(it.key().Area);
        const auto& blocked = writeOnly[it.key().Area];
        View view;
        int end = -1;
        QSet<QString> groupNames;
        QString firstName;

        auto close = [&]() {
            if(view.Length == 0) return;
            // The registers' group, else the first register's name
            const QString part = groupNames.size() == 1 && !groupNames.begin()->isEmpty() ?
                        *groupNames.begin() : firstName;
            view.Title = QString("%1 %2: %3").arg(Model, part, enumToString(view.Mode));
            views.append(view);
        };

        for(auto&& r : regs)
        {
            const int start = r.Address;
            const int stop = start + registerCount(r);
            bool fits = end >= 0
                    && start - end <= MaxGap
                    && qMax(stop, end) - view.Address <= limit
                    && (start - view.Address) % width == 0;
            // The window would grow over an address that is only written
            for(int a = end; fits && a < stop; a++)
                fits = !blocked.contains(a);

            if(!fits) {
                close();
                view = View();
                view.Area = it.key().Area;
                view.Mode = it.key().Mode;
                view.Order = it.key().Order;
                view.Address = r.Address;
                end = start;
                groupNames.clear();
                firstName = r.Name;
            }

            end = qMax(end, stop);
            view.Length = static_cast<quint16>(end - view.Address);
            groupNames.insert(r.Group);

            auto& text = view.Descriptions[r.Address];
            text += (text.isEmpty() ? QString() : QStringLiteral("; ")) + describe(r);
        }
        close();
    }

    std::stable_sort(views.begin(), views.end(), [](const View& a, const View& b) {
        return std::tie(a.Area, a.Address) < std::tie(b.Area, b.Address);
    });
    return views;
}

///
/// \brief Where a window of the map starts in the numbering of the map
/// \param view A view of this map
/// \param addressBase The map's address_base (0 or 1)
/// \return The map's numbering, which is 1-based when address_base is 1.
/// The last wire address (65535) is address 65536 then, which a
/// 16-bit start address cannot hold: the window counts from 0 instead.
///
MbMap::Placement MbMap::placement(const View& view, int addressBase)
{
    const int address = view.Address + addressBase;
    if(address > 65535)
        return { true, view.Address };
    return { addressBase == 0, static_cast<quint16>(address) };
}

///
/// \brief Sets up a window's display definition for a view of the map
/// \param view A view of this map
/// \param addressBase The map's address_base (0 or 1)
/// \param dd The window's display definition: its area, start address,
/// numbering and length are set, and its address space becomes 6 digits
/// when the view lies past the end of a 5-digit one
///
void MbMap::setUpWindow(const View& view, int addressBase, DisplayDefinition& dd)
{
    const auto start = placement(view, addressBase);
    dd.PointType = view.Area;
    dd.ZeroBasedAddress = start.ZeroBased;
    dd.PointAddress = start.Address;
    dd.Length = view.Length;
    if(!ModbusLimits::isReadable(dd.PointAddress, dd.ZeroBasedAddress, dd.Length, dd.AddrSpace))
        dd.AddrSpace = AddressSpace::Addr6Digits;
}

///
/// \brief Where a map of the library is kept inside a folder
/// \param directory The folder the maps are kept in
/// \param path The map's path in the library's index, e.g. "acme/em-100.mbmap.json"
/// \return The file's path in the folder; empty when the path is not a
/// relative path to a .mbmap.json file that stays inside the folder
/// (the index is not trusted: "../x.mbmap.json" is refused)
///
QString MbMap::cachedFilePath(const QString& directory, const QString& path)
{
    if(!isMapFile(path) || path.contains(QLatin1Char('\\')) || path.contains(QLatin1Char(':')) || QDir::isAbsolutePath(path))
        return QString();

    const QString root = QDir::cleanPath(QDir(directory).absolutePath());
    const QString filename = QDir::cleanPath(root + QLatin1Char('/') + path);
    return filename.startsWith(root + QLatin1Char('/')) ? filename : QString();
}
