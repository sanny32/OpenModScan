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
    /// \param timestamp
    /// \param deviceId
    ///
    ReadDiscreteInputsRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
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
        return ModbusMessage::isValid() && dataSize() == 4;
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
    /// \param timestamp
    /// \param deviceId
    ///
    ReadDiscreteInputsResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief ReadDiscreteInputsResponse
    /// \param adu
    /// \param timestamp
    ///
    ReadDiscreteInputsResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::ReadDiscreteInputs);
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
