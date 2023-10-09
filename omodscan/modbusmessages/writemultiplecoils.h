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
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleCoils);
    }

    ///
    /// \brief WriteMultipleCoilsRequest
    /// \param adu
    /// \param timestamp
    ///
    WriteMultipleCoilsRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleCoils);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() > 5;
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeWord(data(1), data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief quantity
    /// \return
    ///
    quint16 quantity() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return data(4);
    }

    ///
    /// \brief values
    /// \return
    ///
    QByteArray values() const {
        return slice(5);
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
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleCoils);
    }

    ///
    /// \brief WriteMultipleCoilsResponse
    /// \param adu
    /// \param timestamp
    ///
    WriteMultipleCoilsResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::WriteMultipleCoils);
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
    /// \brief quantity
    /// \return
    ///
    quint16 quantity() const {
        return makeWord(data(3), data(2), ByteOrder::LittleEndian);
    }
};

#endif // WRITEMULTIPLECOILS_H
