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
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() == 2;
    }

    ///
    /// \brief fifoAddress
    /// \return
    ///
    quint16 fifoAddress() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
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
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadFifoQueue);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() > 5;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint16 byteCount() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief fifoCount
    /// \return
    ///
    quint16 fifoCount() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }

    ///
    /// \brief fifoValue
    /// \return
    ///
    QByteArray fifoValue() const {
        return  _data.right(_data.size() - 4);
    }
};

#endif // READFIFOQUEUE_H
