#ifndef READWRITEMULTIPLEREGISTERS_H
#define READWRITEMULTIPLEREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The ReadWriteMultipleRegistersRequest class
///
class ReadWriteMultipleRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadWriteMultipleRegistersRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    ReadWriteMultipleRegistersRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadWriteMultipleRegisters);
    }

    ///
    /// \brief ReadWriteMultipleRegistersRequest
    /// \param adu
    /// \param timestamp
    ///
    ReadWriteMultipleRegistersRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadWriteMultipleRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               readLength() >= 1 && readLength() <= 0x7D &&
               writeLength() >= 1 && writeLength() <= 0x79 &&
               writeByteCount() > 0 &&
               writeByteCount() == writeValues().size();
    }

    ///
    /// \brief readStartAddress
    /// \return
    ///
    quint16 readStartAddress() const {
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief readLength
    /// \return
    ///
    quint16 readLength() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeStartAddress
    /// \return
    ///
    quint16 writeStartAddress() const {
        return makeWord(data(5), data(4), ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeLength
    /// \return
    ///
    quint16 writeLength() const {
        return makeWord(data(7), data(6), ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeByteCount
    /// \return
    ///
    quint8 writeByteCount() const {
        return data(8);
    }

    ///
    /// \brief writeValues
    /// \return
    ///
    QByteArray writeValues() const {
        return slice(9);
    }
};

///
/// \brief The ReadWriteMultipleRegistersResponse class
///
class ReadWriteMultipleRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadWriteMultipleRegistersResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    ReadWriteMultipleRegistersResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadWriteMultipleRegisters);
    }

    ///
    /// \brief ReadWriteMultipleRegistersResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadWriteMultipleRegistersResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadWriteMultipleRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() > 0 &&
               byteCount() == values().size();
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return data(0);
    }

    ///
    /// \brief values
    /// \return
    ///
    QByteArray values() const {
        return slice(1);
    }
};

#endif // READWRITEMULTIPLEREGISTERS_H
