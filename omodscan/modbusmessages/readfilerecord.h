#ifndef READFILERECORD_H
#define READFILERECORD_H

#include "modbusmessage.h"

///
/// \brief The ReadFileRecordRequest class
///
class ReadFileRecordRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadFileRecordRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    ReadFileRecordRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief ReadFileRecordRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadFileRecordRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() >= 0x07 && byteCount() <= 0xF5;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return ModbusMessage::at(0);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return  ModbusMessage::data(1);
    }
};

///
/// \brief The ReadFileRecordResponse class
///
class ReadFileRecordResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadFileRecordResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    ReadFileRecordResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief ReadFileRecordResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    ReadFileRecordResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               (isException() ? true : (byteCount() >= 0x07 && byteCount() <= 0xF5));
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return ModbusMessage::at(0);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return  ModbusMessage::data(1);
    }
};

#endif // READFILERECORD_H
