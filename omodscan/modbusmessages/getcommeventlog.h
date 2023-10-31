#ifndef GETCOMMEVENTLOG_H
#define GETCOMMEVENTLOG_H

#include "modbusmessage.h"

///
/// \brief The GetCommEventLogRequest class
///
class GetCommEventLogRequest : public ModbusMessage
{
public:
    ///
    /// \brief GetCommEventLogRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    GetCommEventLogRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventLog);
    }

    ///
    /// \brief GetCommEventLogRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    GetCommEventLogRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventLog);
    }
};

///
/// \brief The GetCommEventLogResponse class
///
class GetCommEventLogResponse : public ModbusMessage
{
public:
    ///
    /// \brief GetCommEventLogResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    GetCommEventLogResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventLog);
    }

    ///
    /// \brief GetCommEventLogResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    GetCommEventLogResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventLog);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() > 0 &&
               byteCount() == events().size() - 6;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return at(0);
    }

    ///
    /// \brief status
    /// \return
    ///
    quint16 status() const {
        return makeWord(at(2), at(1), ByteOrder::LittleEndian);
    }

    ///
    /// \brief eventCount
    /// \return
    ///
    quint16 eventCount() const {
        return makeWord(at(4), at(3), ByteOrder::LittleEndian);
    }

    ///
    /// \brief messageCount
    /// \return
    ///
    quint16 messageCount() const {
        return makeWord(at(6), at(5), ByteOrder::LittleEndian);
    }

    ///
    /// \brief events
    /// \return
    ///
    QByteArray events() const {
        return data(7);
    }
};

#endif // GETCOMMEVENTLOG_H
