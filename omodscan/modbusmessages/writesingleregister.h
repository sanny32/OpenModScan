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
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleRegister);
    }

    ///
    /// \brief WriteSingleRegisterRequest
    /// \param adu
    /// \param timestamp
    ///
    WriteSingleRegisterRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief value
    /// \return
    ///
    quint16 value() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
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
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleRegister);
    }

    ///
    /// \brief WriteSingleRegisterResponse
    /// \param adu
    /// \param timestamp
    ///
    WriteSingleRegisterResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
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
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief value
    /// \return
    ///
    quint16 value() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }
};

#endif // WRITESINGLEREGISTER_H
