#ifndef READHOLDINGREGISTERS_H
#define READHOLDINGREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The ReadHoldingRegistersRequest class
///
class ReadHoldingRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadHoldingRegistersRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    ReadHoldingRegistersRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadHoldingRegisters);
    }

    ///
    /// \brief ReadHoldingRegistersRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadHoldingRegistersRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadHoldingRegisters);
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
/// \brief The ReadHoldingRegistersResponse class
///
class ReadHoldingRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadHoldingRegistersResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    ReadHoldingRegistersResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadHoldingRegisters);
    }

    ///
    /// \brief ReadHoldingRegistersResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadHoldingRegistersResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadHoldingRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               (isException() ? true : (byteCount() > 0 && byteCount() == registerValue().size()));
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

#endif // READHOLDINGREGISTERS_H
