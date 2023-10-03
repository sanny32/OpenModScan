#ifndef WRITEMULTIPLEREGISTERS_H
#define WRITEMULTIPLEREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The WriteMultipleRegistersRequest class
///
class WriteMultipleRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief WriteMultipleRegistersRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    WriteMultipleRegistersRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteMultipleRegisters);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && _data.size() > 5;
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief quantity
    /// \return
    ///
    quint16 quantity() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return _data[4];
    }

    ///
    /// \brief values
    /// \return
    ///
    QByteArray values() const {
        return _data.right(_data.size() - 5);
    }
};

///
/// \brief The WriteMultipleRegistersResponse class
///
class WriteMultipleRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteMultipleRegistersResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    WriteMultipleRegistersResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteMultipleRegisters);
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
    /// \brief quantity
    /// \return
    ///
    quint16 quantity() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }
};

#endif // WRITEMULTIPLEREGISTERS_H
