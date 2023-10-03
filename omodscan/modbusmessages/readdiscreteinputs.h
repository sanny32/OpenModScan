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
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() == 4;
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
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
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadDiscreteInputs);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() > 1;
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return _data[0];
    }

    ///
    /// \brief inputStatus
    /// \return
    ///
    QByteArray inputStatus() const {
        return _data.right(_data.size() - 1);
    }
};

#endif // READDISCRETEINPUTS_H
