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
    /// \param timestamp
    /// \param deviceId
    ///
    ReadFileRecordRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
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
        return ModbusMessage::isValid() && dataSize() > 1;
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
    /// \param timestamp
    /// \param deviceId
    ///
    ReadFileRecordResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
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
        return ModbusMessage::isValid() && dataSize() > 1;
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
