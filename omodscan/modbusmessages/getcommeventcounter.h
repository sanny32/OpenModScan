#ifndef GETCOMMEVENTCOUNTER_H
#define GETCOMMEVENTCOUNTER_H

#include "modbusmessage.h"

///
/// \brief The GetCommEventCounterRequest class
///
class GetCommEventCounterRequest : public ModbusMessage
{
public:
    ///
    /// \brief GetCommEventCounterRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    GetCommEventCounterRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::GetCommEventCounter);
    }
};

///
/// \brief The GetCommEventCounterResponse class
///
class GetCommEventCounterResponse : public ModbusMessage
{
public:
    ///
    /// \brief GetCommEventCounterResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    GetCommEventCounterResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::GetCommEventCounter);
    }

    ///
    /// \brief status
    /// \return
    ///
    quint16 status() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief eventCount
    /// \return
    ///
    quint16 eventCount() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }
};

#endif // GETCOMMEVENTCOUNTER_H
