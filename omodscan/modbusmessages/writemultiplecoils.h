#ifndef WRITEMULTIPLECOILS_H
#define WRITEMULTIPLECOILS_H

#include "modbusmessage.h"

///
/// \brief The WriteMultipleCoilsRequest class
///
class WriteMultipleCoilsRequest : public ModbusMessage
{
public:
    ///
    /// \brief WriteMultipleCoilsRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    WriteMultipleCoilsRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleCoils);
    }

    ///
    /// \brief WriteMultipleCoilsRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteMultipleCoilsRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleCoils);
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
/// \brief The WriteMultipleCoilsResponse class
///
class WriteMultipleCoilsResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteMultipleCoilsResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    WriteMultipleCoilsResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleCoils);
    }

    ///
    /// \brief WriteMultipleCoilsResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteMultipleCoilsResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleCoils);
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

#endif // WRITEMULTIPLECOILS_H
