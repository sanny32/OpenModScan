#ifndef READINPUTREGISTERS_H
#define READINPUTREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The ReadInputRegistersRequest class
///
class ReadInputRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadInputRegistersRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadInputRegistersRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadInputRegisters);
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
/// \brief The ReadInputRegistersResponse class
///
class ReadInputRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadInputRegistersResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadInputRegistersResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadInputRegisters);
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
    /// \brief registerValue
    /// \return
    ///
    QByteArray registerValue() const {
        return _data.right(_data.size() - 1);
    }
};

#endif // READINPUTREGISTERS_H
