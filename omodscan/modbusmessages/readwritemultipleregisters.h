#ifndef READWRITEMULTIPLEREGISTERS_H
#define READWRITEMULTIPLEREGISTERS_H

#include "modbusmessage.h"

///
/// \brief The ReadWriteMultipleRegistersRequest class
///
class ReadWriteMultipleRegistersRequest : public ModbusMessage
{
public:
    ///
    /// \brief ReadWriteMultipleRegistersRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadWriteMultipleRegistersRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        : ModbusMessage(pdu, timestamp, deviceId, true)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadWriteMultipleRegisters);
    }

    ///
    /// \brief readStartAddress
    /// \return
    ///
    quint16 readStartAddress() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief readLength
    /// \return
    ///
    quint16 readLength() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeStartAddress
    /// \return
    ///
    quint16 writeStartAddress() const {
        return makeWord(_data[5], _data[4], ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeLength
    /// \return
    ///
    quint16 writeLength() const {
        return makeWord(_data[7], _data[6], ByteOrder::LittleEndian);
    }

    ///
    /// \brief writeByteCount
    /// \return
    ///
    quint8 writeByteCount() const {
        return _data[8];
    }

    ///
    /// \brief writeValues
    /// \return
    ///
    QByteArray writeValues() const {
        return _data.right(_data.size() - 9);
    }
};

///
/// \brief The ReadWriteMultipleRegistersResponse class
///
class ReadWriteMultipleRegistersResponse : public ModbusMessage
{
public:
    ///
    /// \brief ReadWriteMultipleRegistersResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadWriteMultipleRegistersResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId)
        :ModbusMessage(pdu, timestamp, deviceId, false)
    {
        Q_ASSERT((_funcCode & ~QModbusPdu::ExceptionByte) == QModbusPdu::ReadWriteMultipleRegisters);
    }

    ///
    /// \brief byteCount
    /// \return
    ///
    quint8 byteCount() const {
        return _data[0];
    }

    ///
    /// \brief values
    /// \return
    ///
    QByteArray values() const {
        return _data.right(_data.size() - 1);
    }
};

#endif // READWRITEMULTIPLEREGISTERS_H
