#ifndef WRITEMULTIPLEREGISTERS_H
#define WRITEMULTIPLEREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The WriteMultipleRegistersRequest class
///
class WriteMultipleRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief WriteMultipleRegistersRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    WriteMultipleRegistersRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleRegisters);
    }

    ///
    /// \brief WriteMultipleRegistersRequest
    /// \param adu
    /// \param timestamp
    ///
    WriteMultipleRegistersRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() > 0 &&
                byteCount() == values().size();
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief quantity
    /// \return
    ///
    quint16 quantity() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return data(4);
    }

    ///
    /// \brief values
    /// \return
    ///
    QByteArray values() const {
        return slice(5);
    }
};

///
/// \brief The WriteMultipleRegistersResponse class
///
class WriteMultipleRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteMultipleRegistersResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    WriteMultipleRegistersResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleRegisters);
    }

    ///
    /// \brief WriteMultipleRegistersResponse
    /// \param adu
    /// \param timestamp
    ///
    WriteMultipleRegistersResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleRegisters);
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
    /// \brief quantity
    /// \return
    ///
    quint16 quantity() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }
};

#endif // WRITEMULTIPLEREGISTERS_H
