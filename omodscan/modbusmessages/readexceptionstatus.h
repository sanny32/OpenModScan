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
    /// \param timestamp
    /// \param deviceId
    ///
    ReadExceptionStatusRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadExceptionStatus);
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
    /// \param timestamp
    /// \param deviceId
    ///
    ReadExceptionStatusResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadExceptionStatus);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() == 1;
    }

    ///
    /// \brief outputData
    /// \return
    ///
    quint8 outputData() const {
        return _data[0];
    }
};

#endif // READEXCEPTIONSTATUS_H
