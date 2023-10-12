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
    /// \param timestamp
    /// \param checksum
    ///
    WriteFileRecordRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteFileRecord);
    }

    ///
    /// \brief WriteFileRecordRequest
    /// \param adu
    /// \param timestamp
    ///
    WriteFileRecordRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
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
    /// \param timestamp
    /// \param checksum
    ///
    WriteFileRecordResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteFileRecord);
    }

    ///
    /// \brief WriteFileRecordResponse
    /// \param adu
    /// \param timestamp
    ///
    WriteFileRecordResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
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

#endif // WRITEFILERECORD_H
