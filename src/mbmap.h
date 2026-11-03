#ifndef MBMAP_H
#define MBMAP_H

#include <QList>
#include <QMap>
#include <QString>
#include <QModbusDataUnit>
#include "enums.h"
#include "displaydefinition.h"

///
/// \brief A Modbus device map in the .mbmap format, version 2
/// (https://github.com/nobrega8/mbmap-library/blob/main/SPEC.md).
/// Only what Open ModScan can show is read: the registers' areas,
/// addresses, types, byte and word order and access, their names for the
/// address descriptions, and the largest read the device accepts.
///
struct MbMap
{
    ///
    /// \brief One register of the map
    ///
    struct Register
    {
        QString Name;
        QString Group;
        QString Unit;
        QString Description;
        QModbusDataUnit::RegisterType Area = QModbusDataUnit::Invalid;
        quint16 Address = 0;    // as sent on the wire (0-based)
        QString Type;
        quint16 Length = 1;     // string length in registers
        int Bit = -1;
        int Bits = 0;
        double Scale = 1;
        double Offset = 0;
        bool ByteSwapped = false;
        bool WordSwapped = false;
        bool Readable = true;   // false for "access": "W"
    };

    ///
    /// \brief Registers that one Open ModScan window shows: one area, one
    /// data display mode and one byte order, over a block of addresses.
    ///
    struct View
    {
        QString Title;
        QModbusDataUnit::RegisterType Area = QModbusDataUnit::Invalid;
        DataDisplayMode Mode = DataDisplayMode::UInt16;
        ByteOrder Order = ByteOrder::Direct;
        quint16 Address = 0;    // first address, 0-based
        quint16 Length = 0;
        QMap<quint16, QString> Descriptions; // 0-based address -> text
    };

    ///
    /// \brief Where a window starts, as Open ModScan's display definition
    /// takes it
    ///
    struct Placement
    {
        bool ZeroBased = true;  // addresses count from 0 rather than 1
        quint16 Address = 0;    // first address in that numbering
    };

    ///
    /// \brief Addresses of a write-only register that could not be kept as a
    /// register (e.g. a string longer than a read can be), which no window may
    /// read all the same
    ///
    struct WriteOnlyRange
    {
        QModbusDataUnit::RegisterType Area = QModbusDataUnit::Invalid;
        quint16 Address = 0;    // as sent on the wire (0-based)
        int Count = 1;
    };

    QString Vendor;
    QString Model;
    QString MapDescription;
    quint8 UnitId = 1;
    int AddressBase = 0;
    int MaxReadRegisters = 125; // largest read of registers the device accepts (FC03/04)
    int MaxReadBits = 2000;     // largest read of coils or inputs the device accepts (FC01/02)
    QList<Register> Registers;
    QList<WriteOnlyRange> WriteOnlyRanges;

    ///
    /// \brief Reads a map from the content of a .mbmap.json file
    /// \param data The file's content, in UTF-8, with or without a byte order mark
    /// \param error Set to the reason when the content is not a map Open ModScan
    /// can read, cleared otherwise; may be null
    /// \return The map, without the registers that have errors; an empty map on error
    ///
    static MbMap parse(const QByteArray& data, QString* error);

    ///
    /// \brief Reads a map from a .mbmap.json file
    /// \param filename Path of the file
    /// \param error Set to the reason when the file cannot be read or is not a
    /// map Open ModScan can read, cleared otherwise; may be null
    /// \return The map; an empty map on error
    ///
    static MbMap load(const QString& filename, QString* error);

    ///
    /// \brief Tells a device map from other files by its name
    /// \param filename Name or path of a file
    /// \return true when the name ends in .mbmap.json, in any case
    ///
    static bool isMapFile(const QString& filename);

    ///
    /// \brief Where a map of the library is kept inside a folder
    /// \param directory The folder the maps are kept in
    /// \param path The map's path in the library's index, e.g. "acme/em-100.mbmap.json"
    /// \return The file's path in the folder; empty when the path is not a
    /// relative path to a .mbmap.json file that stays inside the folder
    /// (the index is not trusted: "../x.mbmap.json" is refused)
    ///
    static QString cachedFilePath(const QString& directory, const QString& path);

    ///
    /// \brief Lays the readable registers out in Open ModScan windows
    /// \return One view per window, by area and then address. A window holds
    /// registers of one area, display mode and byte order that are close
    /// together and line up with its 2- or 4-register values. It is never
    /// longer than Open ModScan's limit or the map's max_read_registers /
    /// max_read_bits, and never covers an address that is only written.
    /// Registers longer than those limits on their own are left out.
    ///
    QList<View> views() const;

    ///
    /// \brief Where a window of the map starts in the numbering of the map
    /// \param view A view of this map
    /// \param addressBase The map's address_base (0 or 1)
    /// \return The map's numbering, which is 1-based when address_base is 1.
    /// The last wire address (65535) is address 65536 then, which a
    /// 16-bit start address cannot hold: the window counts from 0 instead.
    ///
    static Placement placement(const View& view, int addressBase);

    ///
    /// \brief Sets up a window's display definition for a view of the map
    /// \param view A view of this map
    /// \param addressBase The map's address_base (0 or 1)
    /// \param dd The window's display definition: its area, start address,
    /// numbering and length are set, and its address space becomes 6 digits
    /// when the view lies past the end of a 5-digit one
    ///
    static void setUpWindow(const View& view, int addressBase, DisplayDefinition& dd);

    ///
    /// \brief The map's name for window titles and dialogs
    /// \return The vendor and model, e.g. "Example Corp EM-100"
    ///
    QString title() const;
};

#endif // MBMAP_H
