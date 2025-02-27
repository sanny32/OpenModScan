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
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    GetCommEventCounterRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventCounter);
    }

    ///
    /// \brief GetCommEventCounterRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    GetCommEventCounterRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventCounter);
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
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    ///
    GetCommEventCounterResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventCounter);
    }

    ///
    /// \brief GetCommEventCounterResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    GetCommEventCounterResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventCounter);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() == 4;
    }

    ///
    /// \brief status
    /// \return
    ///
    quint16 status() const {
        return makeUInt16(at(1), at(0), ByteOrder::Direct);
    }

    ///
    /// \brief eventCount
    /// \return
    ///
    quint16 eventCount() const {
        return makeUInt16(at(3), at(2), ByteOrder::Direct);
    }
};

#endif // GETCOMMEVENTCOUNTER_H
