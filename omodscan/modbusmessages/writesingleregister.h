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
    /// \param timestamp
    /// \param deviceId
    ///
    WriteSingleRegisterRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteSingleRegister);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() == 4;
    }

    ///
    /// \brief address
    /// \return
    ///
    quint16 address() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief value
    /// \return
    ///
    quint16 value() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
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
    /// \param timestamp
    /// \param deviceId
    ///
    WriteSingleRegisterResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteSingleRegister);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() == 4;
    }

    ///
    /// \brief address
    /// \return
    ///
    quint16 address() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief value
    /// \return
    ///
    quint16 value() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }
};

#endif // WRITESINGLEREGISTER_H
