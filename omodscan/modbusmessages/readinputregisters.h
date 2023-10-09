#ifndef READINPUTREGISTERS_H
#define READINPUTREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The ReadInputRegistersRequest class
///
class ReadInputRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadInputRegistersRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadInputRegistersRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadInputRegisters);
    }

    ///
    /// \brief ReadInputRegistersRequest
    /// \param adu
    /// \param timestamp
    ///
    ReadInputRegistersRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadInputRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() == 4;
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }
};

///
/// \brief The ReadInputRegistersResponse class
///
class ReadInputRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadInputRegistersResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadInputRegistersResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadInputRegisters);
    }

    ///
    /// \brief ReadInputRegistersResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadInputRegistersResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadInputRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() > 1;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return data(0);
    }

    ///
    /// \brief registerValue
    /// \return
    ///
    QByteArray registerValue() const {
        return slice(1);
    }
};

#endif // READINPUTREGISTERS_H
