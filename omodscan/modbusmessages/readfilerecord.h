#ifndef READFILERECORD_H
#define READFILERECORD_H

#include "modbusmessage.h"

///
/// \brief The ReadFileRecordRequest class
///
class ReadFileRecordRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadFileRecordRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadFileRecordRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return _data[0];
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return  _data.right(_data.size() - 1);
    }
};

///
/// \brief The ReadFileRecordResponse class
///
class ReadFileRecordResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadFileRecordResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadFileRecordResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return _data[0];
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return  _data.right(_data.size() - 1);
    }
};

#endif // READFILERECORD_H
