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
    /// \param timestamp
    /// \param deviceId
    ///
    WriteSingleCoilRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteSingleCoil);
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
/// \brief The WriteSingleCoilResponse class
///
class WriteSingleCoilResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteSingleCoilResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    WriteSingleCoilResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteSingleCoil);
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

#endif // WRITESINGLECOIL_H
