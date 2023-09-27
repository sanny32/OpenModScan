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
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::MaskWriteRegister);
    }

    ///
    /// \brief address
    /// \return
    ///
    quint16 address() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief andMask
    /// \return
    ///
    quint16 andMask() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }

    ///
    /// \brief orMask
    /// \return
    ///
    quint16 orMask() const {
        return makeWord(_data[5], _data[4], ByteOrder::LittleEndian);
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
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::MaskWriteRegister);
    }

    ///
    /// \brief address
    /// \return
    ///
    quint16 address() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief andMask
    /// \return
    ///
    quint16 andMask() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }

    ///
    /// \brief orMask
    /// \return
    ///
    quint16 orMask() const {
        return makeWord(_data[5], _data[4], ByteOrder::LittleEndian);
    }
};

#endif // MASKWRITEREGISTER_H
