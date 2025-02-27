#ifndef READINPUTREGISTERS_H
#define READINPUTREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The ReadInputRegistersRequest class
///
class ReadInputRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadInputRegistersRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    ReadInputRegistersRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadInputRegisters);
    }

    ///
    /// \brief ReadInputRegistersRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadInputRegistersRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadInputRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               length() >= 1 && length() <= 0x7D;
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeUInt16(at(1), at(0), ByteOrder::Direct);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        return makeUInt16(at(3), at(2), ByteOrder::Direct);
    }
};

///
/// \brief The ReadInputRegistersResponse class
///
class ReadInputRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadInputRegistersResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
     ///
    ReadInputRegistersResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadInputRegisters);
    }

    ///
    /// \brief ReadInputRegistersResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadInputRegistersResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadInputRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() > 0 &&
               byteCount() == registerValue().size();
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return at(0);
    }

    ///
    /// \brief registerValue
    /// \return
    ///
    QByteArray registerValue() const {
        return data(1);
    }
};

#endif // READINPUTREGISTERS_H
