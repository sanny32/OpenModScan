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
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    ReportServerIdRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReportServerId);
    }

    ///
    /// \brief ReportServerIdRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReportServerIdRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReportServerId);
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
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    ReportServerIdResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReportServerId);
    }

    ///
    /// \brief ReportServerIdResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReportServerIdResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReportServerId);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() > 1;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return ModbusMessage::at(0);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return  ModbusMessage::data(1);
    }
};

#endif // REPORTSERVERID_H
