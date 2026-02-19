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
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    ReadFifoQueueRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief ReadFifoQueueRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadFifoQueueRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
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
        return makeUInt16(at(1), at(0), ByteOrder::Direct);
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
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    ReadFifoQueueResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief ReadFifoQueueResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadFifoQueueResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && (isException() ? true : (fifoCount() <= 31 && fifoCount() == fifoValue().size()));
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint16 byteCount() const {
        return makeUInt16(at(1), at(0), ByteOrder::Direct);
    }

    ///
    /// \brief fifoCount
    /// \return
    ///
    quint16 fifoCount() const {
        return makeUInt16(at(3), at(2), ByteOrder::Direct);
    }

    ///
    /// \brief fifoValue
    /// \return
    ///
    QByteArray fifoValue() const {
        return  data(4);
    }
};

#endif // READFIFOQUEUE_H
