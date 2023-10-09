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
    /// \param timestamp
    /// \param deviceId
    ///
    WriteFileRecordRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
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
        return ModbusMessage::isValid() && dataSize() > 1;
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
    /// \param timestamp
    /// \param deviceId
    ///
    WriteFileRecordResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
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
        return ModbusMessage::isValid() && dataSize() > 1;
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
