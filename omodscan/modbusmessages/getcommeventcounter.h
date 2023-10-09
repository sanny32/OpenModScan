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
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventCounter);
    }

    ///
    /// \brief GetCommEventCounterRequest
    /// \param adu
    /// \param timestamp
    ///
    GetCommEventCounterRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
    /// \param timestamp
    /// \param deviceId
    ///
    GetCommEventCounterResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::GetCommEventCounter);
    }

    ///
    /// \brief GetCommEventCounterResponse
    /// \param adu
    /// \param timestamp
    ///
    GetCommEventCounterResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
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
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief eventCount
    /// \return
    ///
    quint16 eventCount() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }
};

#endif // GETCOMMEVENTCOUNTER_H
