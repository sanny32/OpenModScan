#ifndef READHOLDINGREGISTERS_H
#define READHOLDINGREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The ReadHoldingRegistersRequest class
///
class ReadHoldingRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadHoldingRegistersRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadHoldingRegistersRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadHoldingRegisters);
    }

    ///
    /// \brief ReadHoldingRegistersRequest
    /// \param adu
    /// \param timestamp
    ///
    ReadHoldingRegistersRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadHoldingRegisters);
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
/// \brief The ReadHoldingRegistersResponse class
///
class ReadHoldingRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadHoldingRegistersResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadHoldingRegistersResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadHoldingRegisters);
    }

    ///
    /// \brief ReadHoldingRegistersResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadHoldingRegistersResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadHoldingRegisters);
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

#endif // READHOLDINGREGISTERS_H
