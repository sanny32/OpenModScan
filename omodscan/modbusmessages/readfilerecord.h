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
    /// \param timestamp
    /// \param checksum
    ///
    ReadFileRecordRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief ReadFileRecordRequest
    /// \param adu
    /// \param timestamp
    ///
    ReadFileRecordRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
        return ModbusMessage::data(0);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return  slice(1);
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
    /// \param timestamp
    /// \param checksum
    ///
    ReadFileRecordResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief ReadFileRecordResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadFileRecordResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadFileRecord);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() >= 0x07 && byteCount() <= 0xF5;;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return ModbusMessage::data(0);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return  slice(1);
    }
};

#endif // READFILERECORD_H
