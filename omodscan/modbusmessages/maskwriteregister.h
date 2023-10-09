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
    /// \param timestamp
    /// \param deviceId
    ///
    MaskWriteRegisterRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::MaskWriteRegister);
    }

    ///
    /// \brief MaskWriteRegisterRequest
    /// \param adu
    /// \param timestamp
    ///
    MaskWriteRegisterRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief andMask
    /// \return
    ///
    quint16 andMask() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }

    ///
    /// \brief orMask
    /// \return
    ///
    quint16 orMask() const {
        return makeWord(data(5), data(4), ByteOrder::LittleEndian);
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
    /// \param timestamp
    /// \param deviceId
    ///
    MaskWriteRegisterResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::MaskWriteRegister);
    }

    ///
    /// \brief MaskWriteRegisterResponse
    /// \param adu
    /// \param timestamp
    ///
    MaskWriteRegisterResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief andMask
    /// \return
    ///
    quint16 andMask() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }

    ///
    /// \brief orMask
    /// \return
    ///
    quint16 orMask() const {
        return makeWord(data(5), data(4), ByteOrder::LittleEndian);
    }
};

#endif // MASKWRITEREGISTER_H
