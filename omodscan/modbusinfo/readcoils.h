#ifndef READCOILS_H
#define READCOILS_H

#include "numericutils.h"
#include "modbuspduinfo.h"

///
/// \brief The ReadCoilsRequest class
///
class ReadCoilsRequest : public ModbusPduInfo
{
    Q_OBJECT
public:
    ///
    /// \brief ReadCoilsRequest
    ///
    explicit ReadCoilsRequest() = default;

    ///
    /// \brief ReadCoilsRequest
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadCoilsRequest(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, QObject* parent)
        : ModbusPduInfo(pdu, timestamp, deviceId, true, parent)
    {
        Q_ASSERT(_funcCode == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        return makeWord(_data[1], _data[2], ByteOrder::LittleEndian);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        return makeWord(_data[3], _data[4], ByteOrder::LittleEndian);
    }
};

///
/// \brief The ReadCoilsResponse class
///
class ReadCoilsResponse : public ModbusPduInfo
{
    Q_OBJECT
public:
    ///
    /// \brief ReadCoilsResponse
    ///
    explicit ReadCoilsResponse() = default;

    ///
    /// \brief ReadCoilsResponse
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    ///
    ReadCoilsResponse(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, QObject* parent)
        :ModbusPduInfo(pdu, timestamp, deviceId, false, parent)
    {
        Q_ASSERT(_funcCode == QModbusPdu::ReadCoils);
    }

    ///
    /// \brief bytesCount
    /// \return
    ///
    quint8 bytesCount() const {
        return _data[1];
    }

    ///
    /// \brief coilsStatus
    /// \return
    ///
    QByteArray coilsStatus() const {
        return QByteArray(_data + 2, _data.size() - 2);
    }
};

#endif // READCOILS_H
