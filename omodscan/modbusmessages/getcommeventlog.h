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
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::GetCommEventLog);
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
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::GetCommEventLog);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() > 7;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return _data[0];
    }

    ///
    /// \brief status
    /// \return
    ///
    quint16 status() const {
        return makeWord(_data[2], _data[1], ByteOrder::LittleEndian);
    }

    ///
    /// \brief eventCount
    /// \return
    ///
    quint16 eventCount() const {
        return makeWord(_data[4], _data[3], ByteOrder::LittleEndian);
    }

    ///
    /// \brief messageCount
    /// \return
    ///
    quint16 messageCount() const {
        return makeWord(_data[6], _data[5], ByteOrder::LittleEndian);
    }

    ///
    /// \brief events
    /// \return
    ///
    QByteArray events() const {
        return _data.right(_data.size() - 7);
    }
};

#endif // GETCOMMEVENTLOG_H
