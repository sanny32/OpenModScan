#ifndef READHOLDINGREGISTERS_H
#define READHOLDINGREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The ReadHoldingRegistersRequest class
///
class ReadHoldingRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadHoldingRegistersRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadHoldingRegistersRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadHoldingRegisters);
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
/// \brief The ReadHoldingRegistersResponse class
///
class ReadHoldingRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadHoldingRegistersResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadHoldingRegistersResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadHoldingRegisters);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return _data[0];
    }

    ///
    /// \brief registerValue
    /// \return
    ///
    QByteArray registerValue() const {
        return _data.right(_data.size() - 1);
    }
};

#endif // READHOLDINGREGISTERS_H
