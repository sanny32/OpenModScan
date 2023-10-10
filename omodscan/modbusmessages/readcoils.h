#ifndef READCOILS_H
#define READCOILS_H

#include "numericutils.h"
#include "modbusmessage.h"

///
/// \brief The ReadCoilsRequest class
///
class ReadCoilsRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadCoilsRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    ReadCoilsRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief ReadCoilsRequest
    /// \param adu
    /// \param timestamp
    ///
    ReadCoilsRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               length() >= 1 && length() <= 0x7D0;
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }
};

///
/// \brief The ReadCoilsResponse class
///
class ReadCoilsResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadCoilsResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    ReadCoilsResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief ReadCoilsResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadCoilsResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() > 0 &&
               (byteCount() == coilStatus().size() || byteCount() == coilStatus().size() - 1);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return data(0);
    }

    ///
    /// \brief coilStatus
    /// \return
    ///
    QByteArray coilStatus() const {
        return slice(1);
    }
};

#endif // READCOILS_H
