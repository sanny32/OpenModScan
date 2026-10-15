#ifndef MBMAP_H
#define MBMAP_H

#include <QList>
#include <QMap>
#include <QString>
#include <QModbusDataUnit>
#include "enums.h"

///
/// \brief A Modbus device map in the .mbmap format, version 2
/// (https://github.com/nobrega8/mbmap-library/blob/main/SPEC.md).
/// Only what Open ModScan can show is read: the registers' areas,
/// addresses, types, byte and word order, and their names for the
/// address descriptions.
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

    QString Vendor;
    QString Model;
    QString MapDescription;
    quint8 UnitId = 1;
    int AddressBase = 0;
    QList<Register> Registers;

    static MbMap parse(const QByteArray& data, QString* error);
    static MbMap load(const QString& filename, QString* error);

    static bool isMapFile(const QString& filename);

    QList<View> views() const;
    QString title() const;
};

#endif // MBMAP_H
