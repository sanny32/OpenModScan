#ifndef READDISCRETEINPUTS_H
#define READDISCRETEINPUTS_H

#include "modbusmessage.h"

///
/// \brief The ReadDiscreteInputsRequest class
///
class ReadDiscreteInputsRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadDiscreteInputsRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    ReadDiscreteInputsRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief ReadDiscreteInputsRequest
    /// \param adu
    /// \param timestamp
    ///
    ReadDiscreteInputsRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
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
/// \brief The ReadDiscreteInputsResponse class
///
class ReadDiscreteInputsResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadDiscreteInputsResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    ReadDiscreteInputsResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief ReadDiscreteInputsResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadDiscreteInputsResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() &&
               byteCount() > 0 &&
               (byteCount() == inputStatus().size() || byteCount() == inputStatus().size() - 1);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return data(0);
    }

    ///
    /// \brief inputStatus
    /// \return
    ///
    QByteArray inputStatus() const {
        return slice(1);
    }
};

#endif // READDISCRETEINPUTS_H
