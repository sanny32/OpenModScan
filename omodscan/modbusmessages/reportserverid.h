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
    /// \param checksum
    ///
    ReportServerIdRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReportServerId);
    }

    ///
    /// \brief ReportServerIdRequest
    /// \param adu
    /// \param timestamp
    ///
    ReportServerIdRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
    /// \param timestamp
    /// \param checksum
    ///
    ReportServerIdResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReportServerId);
    }

    ///
    /// \brief ReportServerIdResponse
    /// \param adu
    /// \param timestamp
    ///
    ReportServerIdResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
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
        return ModbusMessage::data(0);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return  slice(1);
    }
};

#endif // REPORTSERVERID_H
