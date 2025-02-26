#ifndef READDISCRETEINPUTS_H
#define READDISCRETEINPUTS_H

#include "modbusmessage.h"

///
/// \brief The ReadDiscreteInputsRequest class
///
class ReadDiscreteInputsRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadDiscreteInputsRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    ReadDiscreteInputsRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief ReadDiscreteInputsRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadDiscreteInputsRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               length() >= 1 && length() <= 0x7D0;
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
/// \brief The ReadDiscreteInputsResponse class
///
class ReadDiscreteInputsResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadDiscreteInputsResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    ReadDiscreteInputsResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief ReadDiscreteInputsResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadDiscreteInputsResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() > 0 &&
               (byteCount() == inputStatus().size() || byteCount() == inputStatus().size() - 1);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return at(0);
    }

    ///
    /// \brief inputStatus
    /// \return
    ///
    QByteArray inputStatus() const {
        return data(1);
    }
};

#endif // READDISCRETEINPUTS_H
