#ifndef WRITEMULTIPLECOILS_H
#define WRITEMULTIPLECOILS_H

#include "modbusmessage.h"

///
/// \brief The WriteMultipleCoilsRequest class
///
class WriteMultipleCoilsRequest : public ModbusMessage
{
public:
    ///
    /// \brief WriteMultipleCoilsRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    WriteMultipleCoilsRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteMultipleCoils);
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
/// \brief The WriteMultipleCoilsResponse class
///
class WriteMultipleCoilsResponse : public ModbusMessage
{
public:
    ///
    /// \brief WriteMultipleCoilsResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    WriteMultipleCoilsResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::WriteMultipleCoils);
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

#endif // WRITEMULTIPLECOILS_H
