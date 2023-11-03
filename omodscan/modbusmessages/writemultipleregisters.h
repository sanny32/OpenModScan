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
    ///
    WriteMultipleRegistersRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleRegisters);
    }

    ///
    /// \brief WriteMultipleRegistersRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteMultipleRegistersRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
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
        return makeWord(at(1), at(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief quantity
    /// \return
    ///
    quint16 quantity() const {
        return makeWord(at(3), at(2), ByteOrder::LittleEndian);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return at(4);
    }

    ///
    /// \brief values
    /// \return
    ///
    QByteArray values() const {
        return data(5);
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
    ///
    WriteMultipleRegistersResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleRegisters);
    }

    ///
    /// \brief WriteMultipleRegistersResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteMultipleRegistersResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
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
        return makeWord(at(1), at(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief quantity
    /// \return
    ///
    quint16 quantity() const {
        return makeWord(at(3), at(2), ByteOrder::LittleEndian);
    }
};

#endif // WRITEMULTIPLEREGISTERS_H
