#ifndef WRITEFILERECORD_H
#define WRITEFILERECORD_H

#include "modbusmessage.h"

///
/// \brief The WriteFileRecordRequest class
///
class WriteFileRecordRequest : public ModbusMessage
{
public:
    ///
    /// \brief WriteFileRecordRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    WriteFileRecordRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteFileRecord);
    }

    ///
    /// \brief WriteFileRecordRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteFileRecordRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteFileRecord);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               length() >= 0x09 &&
               length() <= 0xFB;
    }

    ///
    /// \brief length
    /// \return
    ///
    quint8 length() const {
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
/// \brief The WriteFileRecordResponse class
///
class WriteFileRecordResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteFileRecordResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param transactionId
    /// \param timestamp
    ///
    WriteFileRecordResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteFileRecord);
    }

    ///
    /// \brief WriteFileRecordResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    WriteFileRecordResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteFileRecord);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               length() >= 0x09 &&
               length() <= 0xFB;
    }

    ///
    /// \brief length
    /// \return
    ///
    quint8 length() const {
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

#endif // WRITEFILERECORD_H
