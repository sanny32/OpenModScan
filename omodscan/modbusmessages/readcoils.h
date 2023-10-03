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
    /// \param timestamp
    /// \param deviceId
    ///
    ReadCoilsRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadCoils);
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
/// \brief The ReadCoilsResponse class
///
class ReadCoilsResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadCoilsResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadCoilsResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadCoils);
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
    /// \brief coilStatus
    /// \return
    ///
    QByteArray coilStatus() const {
        return _data.right(_data.size() - 1);
    }
};

#endif // READCOILS_H
