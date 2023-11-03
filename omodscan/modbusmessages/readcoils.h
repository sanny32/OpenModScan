#ifndef READCOILS_H
#define READCOILS_H

#include "numericutils.h"
#include "modbusmessage.h"

///
/// \brief The ReadCoilsRequest class
///
class ReadCoilsRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadCoilsRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    ReadCoilsRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief ReadCoilsRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadCoilsRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               length() >= 1 && length() <= 0x7D0;
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeWord(at(1), at(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        return makeWord(at(3), at(2), ByteOrder::LittleEndian);
    }
};

///
/// \brief The ReadCoilsResponse class
///
class ReadCoilsResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadCoilsResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    ReadCoilsResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief ReadCoilsResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadCoilsResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() > 0 &&
               (byteCount() == coilStatus().size() || byteCount() == coilStatus().size() - 1);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return at(0);
    }

    ///
    /// \brief coilStatus
    /// \return
    ///
    QByteArray coilStatus() const {
        return data(1);
    }
};

#endif // READCOILS_H
