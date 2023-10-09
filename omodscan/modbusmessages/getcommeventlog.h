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
    /// \param timestamp
    /// \param deviceId
    ///
    GetCommEventLogRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventLog);
    }

    ///
    /// \brief GetCommEventLogRequest
    /// \param adu
    /// \param timestamp
    ///
    GetCommEventLogRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
    /// \param timestamp
    /// \param deviceId
    ///
    GetCommEventLogResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventLog);
    }

    ///
    /// \brief GetCommEventLogResponse
    /// \param adu
    /// \param timestamp
    ///
    GetCommEventLogResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventLog);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() > 7;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return data(0);
    }

    ///
    /// \brief status
    /// \return
    ///
    quint16 status() const {
        return makeWord(data(2), data(1), ByteOrder::LittleEndian);
    }

    ///
    /// \brief eventCount
    /// \return
    ///
    quint16 eventCount() const {
        return makeWord(data(4), data(3), ByteOrder::LittleEndian);
    }

    ///
    /// \brief messageCount
    /// \return
    ///
    quint16 messageCount() const {
        return makeWord(data(6), data(5), ByteOrder::LittleEndian);
    }

    ///
    /// \brief events
    /// \return
    ///
    QByteArray events() const {
        return slice(7);
    }
};

#endif // GETCOMMEVENTLOG_H
