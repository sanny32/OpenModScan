#ifndef READFIFOQUEUE_H
#define READFIFOQUEUE_H

#include "modbusmessage.h"

///
/// \brief The ReadFifoQueueRequest class
///
class ReadFifoQueueRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadFifoQueueRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadFifoQueueRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief ReadFifoQueueRequest
    /// \param adu
    /// \param timestamp
    ///
    ReadFifoQueueRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() == 2;
    }

    ///
    /// \brief fifoAddress
    /// \return
    ///
    quint16 fifoAddress() const {
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }
};

///
/// \brief The ReadFifoQueueResponse class
///
class ReadFifoQueueResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadFifoQueueResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadFifoQueueResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief ReadFifoQueueResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadFifoQueueResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() > 5;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint16 byteCount() const {
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief fifoCount
    /// \return
    ///
    quint16 fifoCount() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }

    ///
    /// \brief fifoValue
    /// \return
    ///
    QByteArray fifoValue() const {
        return  slice(4);
    }
};

#endif // READFIFOQUEUE_H
