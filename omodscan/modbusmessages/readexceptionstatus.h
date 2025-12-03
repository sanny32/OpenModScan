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
    /// \param transactionId
    /// \param timestamp
    ///
    ReadExceptionStatusRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadExceptionStatus);
    }

    ///
    /// \brief ReadExceptionStatusRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadExceptionStatusRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
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
    /// \param transactionId
    /// \param timestamp
    ///
    ReadExceptionStatusResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadExceptionStatus);
    }

    ///
    /// \brief ReadExceptionStatusResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadExceptionStatusResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadExceptionStatus);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && (isException() ? true : (dataSize() == 1));
    }

    ///
    /// \brief outputData
    /// \return
    ///
    quint8 outputData() const {
        return at(0);
    }
};

#endif // READEXCEPTIONSTATUS_H
