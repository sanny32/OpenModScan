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
    /// \param checksum
    ///
    WriteSingleCoilRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleCoil);
    }

    ///
    /// \brief WriteSingleCoilRequest
    /// \param adu
    /// \param timestamp
    ///
    WriteSingleCoilRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
    /// \param checksum
    ///
    WriteSingleCoilResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteSingleCoil);
    }

    ///
    /// \brief WriteSingleCoilResponse
    /// \param adu
    /// \param timestamp
    ///
    WriteSingleCoilResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
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

#endif // WRITESINGLECOIL_H
