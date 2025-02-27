#ifndef WRITESINGLECOIL_H
#define WRITESINGLECOIL_H

#include "modbusmessage.h"

///
/// \brief The WriteSingleCoilRequest class
///
class WriteSingleCoilRequest : public ModbusMessage
{
public:
    ///
    /// \brief WriteSingleCoilRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    WriteSingleCoilRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleCoil);
    }

    ///
    /// \brief WriteSingleCoilRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteSingleCoilRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleCoil);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               (value() == 0 || value() == 0xFF00);
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
/// \brief The WriteSingleCoilResponse class
///
class WriteSingleCoilResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteSingleCoilResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    WriteSingleCoilResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleCoil);
    }

    ///
    /// \brief WriteSingleCoilResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteSingleCoilResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleCoil);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               (value() == 0 || value() == 0xFF00);
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

#endif // WRITESINGLECOIL_H
