#ifndef MASKWRITEREGISTER_H
#define MASKWRITEREGISTER_H

#include "modbusmessage.h"

///
/// \brief The MaskWriteRegisterRequest class
///
class MaskWriteRegisterRequest : public ModbusMessage
{
public:
    ///
    /// \brief MaskWriteRegisterRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    MaskWriteRegisterRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::MaskWriteRegister);
    }

    ///
    /// \brief MaskWriteRegisterRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    MaskWriteRegisterRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::MaskWriteRegister);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() == 6;
    }

    ///
    /// \brief address
    /// \return
    ///
    quint16 address() const {
        return makeUInt16(at(1), at(0), ByteOrder::Direct);
    }

    ///
    /// \brief andMask
    /// \return
    ///
    quint16 andMask() const {
        return makeUInt16(at(3), at(2), ByteOrder::Direct);
    }

    ///
    /// \brief orMask
    /// \return
    ///
    quint16 orMask() const {
        return makeUInt16(at(5), at(4), ByteOrder::Direct);
    }
};

///
/// \brief The MaskWriteRegisterResponse class
///
class MaskWriteRegisterResponse : public ModbusMessage
{
public:
    ///
    /// \brief MaskWriteRegisterResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    MaskWriteRegisterResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::MaskWriteRegister);
    }

    ///
    /// \brief MaskWriteRegisterResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    MaskWriteRegisterResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::MaskWriteRegister);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() == 6;
    }

    ///
    /// \brief address
    /// \return
    ///
    quint16 address() const {
        return makeUInt16(at(1), at(0), ByteOrder::Direct);
    }

    ///
    /// \brief andMask
    /// \return
    ///
    quint16 andMask() const {
        return makeUInt16(at(3), at(2), ByteOrder::Direct);
    }

    ///
    /// \brief orMask
    /// \return
    ///
    quint16 orMask() const {
        return makeUInt16(at(5), at(4), ByteOrder::Direct);
    }
};

#endif // MASKWRITEREGISTER_H
