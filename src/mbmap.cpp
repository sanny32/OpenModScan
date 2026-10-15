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
/// \brief MbMap::isMapFile
/// \param filename
/// \return true for a .mbmap.json file
///
bool MbMap::isMapFile(const QString& filename)
{
    return filename.endsWith(".mbmap.json", Qt::CaseInsensitive);
}

///
/// \brief MbMap::load
/// \param filename
/// \param error
/// \return
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
/// \brief MbMap::parse
/// \param data The file's content
/// \param error Set when the file is not a map Open ModScan can read
/// \return The map; registers with errors are left out
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

        if(r.Type == QLatin1String("string")) {
            if(!toInteger(obj.value("length"), n) || n < 1 || n > 125)
                continue;
            r.Length = static_cast<quint16>(n);
        }
        if(obj.contains("bit") && toInteger(obj.value("bit"), n) && n >= 0 && n <= 15)
            r.Bit = static_cast<int>(n);
        if(!bitArea && r.Type == QLatin1String("bool") && r.Bit < 0)
            continue;
        if(r.Type == QLatin1String("uint16") && r.Bit >= 0 && obj.contains("bits")
           && toInteger(obj.value("bits"), n) && n >= 1 && r.Bit + n <= 16)
            r.Bits = static_cast<int>(n);

        if(obj.value("scale").isDouble()) r.Scale = obj.value("scale").toDouble();
        if(obj.value("offset").isDouble()) r.Offset = obj.value("offset").toDouble();

        r.ByteSwapped = obj.contains("byte_order") ? obj.value("byte_order").toString() == QLatin1String("little") : fileByteSwap;
        r.WordSwapped = obj.contains("word_order") ? obj.value("word_order").toString() == QLatin1String("little") : fileWordSwap;

        if(static_cast<int>(r.Address) + registerCount(r) > 65536)
            continue;

        map.Registers.append(r);
    }

    if(error) error->clear();
    return map;
}

///
/// \brief MbMap::title
/// \return "Vendor Model"
///
QString MbMap::title() const
{
    return QString("%1 %2").arg(Vendor, Model);
}

///
/// \brief MbMap::views
/// \return The windows that show the map's registers. Registers of one
/// area, display mode and byte order share a window while they are close
/// together, fit Open ModScan's length limit and line up with the window's
/// 2- or 4-register values.
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

    QMap<Key, QList<Register>> groups;
    for(auto&& r : Registers) {
        const auto mode = displayMode(r);
        const bool bitArea = r.Area == QModbusDataUnit::Coils || r.Area == QModbusDataUnit::DiscreteInputs;
        const auto order = (r.ByteSwapped && !bitArea) ? ByteOrder::Swapped : ByteOrder::Direct;
        groups[{ r.Area, mode, order }].append(r);
    }

    const int maxLength = ModbusLimits::lengthRange().to();
    QList<View> views;

    for(auto it = groups.begin(); it != groups.end(); ++it)
    {
        auto regs = it.value();
        std::stable_sort(regs.begin(), regs.end(), [](const Register& a, const Register& b) {
            return a.Address < b.Address;
        });

        const int width = modeWidth(it.key().Mode);
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
            const bool fits = end >= 0
                    && start - end <= MaxGap
                    && qMax(stop, end) - view.Address <= maxLength
                    && (start - view.Address) % width == 0;

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
