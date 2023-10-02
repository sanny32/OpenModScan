#ifndef REPORTSERVERID_H
#define REPORTSERVERID_H

#include "modbusmessage.h"

///
/// \brief The ReportServerIdRequest class
///
class ReportServerIdRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReportServerIdRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReportServerIdRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReportServerId);
    }
};

///
/// \brief The ReportServerIdResponse class
///
class ReportServerIdResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReportServerIdResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReportServerIdResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReportServerId);
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

#endif // REPORTSERVERID_H
