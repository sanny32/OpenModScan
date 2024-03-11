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
    ///
    ReadWriteMultipleRegistersRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadWriteMultipleRegisters);
    }

    ///
    /// \brief ReadWriteMultipleRegistersRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadWriteMultipleRegistersRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
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
        return makeUInt16(at(1), at(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief readLength
    /// \return
    ///
    quint16 readLength() const {
        return makeUInt16(at(3), at(2), ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeStartAddress
    /// \return
    ///
    quint16 writeStartAddress() const {
        return makeUInt16(at(5), at(4), ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeLength
    /// \return
    ///
    quint16 writeLength() const {
        return makeUInt16(at(7), at(6), ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeByteCount
    /// \return
    ///
    quint8 writeByteCount() const {
        return at(8);
    }

    ///
    /// \brief writeValues
    /// \return
    ///
    QByteArray writeValues() const {
        return data(9);
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
    ///
    ReadWriteMultipleRegistersResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadWriteMultipleRegisters);
    }

    ///
    /// \brief ReadWriteMultipleRegistersResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadWriteMultipleRegistersResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
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
        return at(0);
    }

    ///
    /// \brief values
    /// \return
    ///
    QByteArray values() const {
        return data(1);
    }
};

#endif // READWRITEMULTIPLEREGISTERS_H
