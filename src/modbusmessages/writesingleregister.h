#ifndef WRITESINGLEREGISTER_H
#define WRITESINGLEREGISTER_H

#include "modbusmessage.h"

///
/// \brief The WriteSingleRegisterRequest class
///
class WriteSingleRegisterRequest : public ModbusMessage
{
public:
    ///
    /// \brief WriteSingleRegisterRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    WriteSingleRegisterRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleRegister);
    }

    ///
    /// \brief WriteSingleRegisterRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteSingleRegisterRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleRegister);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && (isException() ? true : (dataSize() == 4));
    }

    ///
    /// \brief address
    /// \return
    ///
    quint16 address() const {
        return makeUInt16(at(1), at(0), ByteOrder::Direct);
    }

    ///
    /// \brief value
    /// \return
    ///
    quint16 value() const {
        return makeUInt16(at(3), at(2), ByteOrder::Direct);
    }
};

///
/// \brief The WriteSingleRegisterResponse class
///
class WriteSingleRegisterResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteSingleRegisterResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    WriteSingleRegisterResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleRegister);
    }

    ///
    /// \brief WriteSingleRegisterResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteSingleRegisterResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleRegister);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() == 4;
    }

    ///
    /// \brief address
    /// \return
    ///
    quint16 address() const {
        return makeUInt16(at(1), at(0), ByteOrder::Direct);
    }

    ///
    /// \brief value
    /// \return
    ///
    quint16 value() const {
        return makeUInt16(at(3), at(2), ByteOrder::Direct);
    }
};

#endif // WRITESINGLEREGISTER_H
