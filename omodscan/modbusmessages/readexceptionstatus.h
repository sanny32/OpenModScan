#ifndef READEXCEPTIONSTATUS_H
#define READEXCEPTIONSTATUS_H

#include "modbusmessage.h"

///
/// \brief The ReadExceptionStatusRequest class
///
class ReadExceptionStatusRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadExceptionStatusRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    ReadExceptionStatusRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadExceptionStatus);
    }

    ///
    /// \brief ReadExceptionStatusRequest
    /// \param adu
    /// \param timestamp
    ///
    ReadExceptionStatusRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadExceptionStatus);
    }
};

///
/// \brief The ReadExceptionStatusResponse class
///
class ReadExceptionStatusResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadExceptionStatusResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    ReadExceptionStatusResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadExceptionStatus);
    }

    ///
    /// \brief ReadExceptionStatusResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadExceptionStatusResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadExceptionStatus);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() == 1;
    }

    ///
    /// \brief outputData
    /// \return
    ///
    quint8 outputData() const {
        return data(0);
    }
};

#endif // READEXCEPTIONSTATUS_H
