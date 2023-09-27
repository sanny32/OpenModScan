#ifndef WRITEFILERECORD_H
#define WRITEFILERECORD_H

#include "modbusmessage.h"

///
/// \brief The WriteFileRecordRequest class
///
class WriteFileRecordRequest : public ModbusMessage
{
public:
    ///
    /// \brief WriteFileRecordRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    WriteFileRecordRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteFileRecord);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint8 length() const {
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
/// \brief The WriteFileRecordResponse class
///
class WriteFileRecordResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteFileRecordResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    WriteFileRecordResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteFileRecord);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint8 length() const {
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

#endif // WRITEFILERECORD_H
