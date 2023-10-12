#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "modbusmessage.h"

///
/// \brief The DiagnosticsRequest class
///
class DiagnosticsRequest : public ModbusMessage
{
public:
    ///
    /// \brief DiagnosticsRequest
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    DiagnosticsRequest(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, int checksum)
        : ModbusMessage(pdu, protocol, deviceId, timestamp, true, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::Diagnostics);
    }

    ///
    /// \brief DiagnosticsRequest
    /// \param adu
    /// \param timestamp
    ///
    DiagnosticsRequest(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::Diagnostics);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() > 2;
    }


    ///
    /// \brief subfunc
    /// \return
    ///
    quint16 subfunc() const {
        return makeWord(ModbusMessage::data(1), ModbusMessage::data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return slice(2);
    }
};

///
/// \brief The DiagnosticsResponse class
///
class DiagnosticsResponse : public ModbusMessage
{
public:
    ///
    /// \brief DiagnosticsResponse
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param checksum
    ///
    DiagnosticsResponse(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId,  const QDateTime& timestamp, int checksum)
        :ModbusMessage(pdu, protocol, deviceId, timestamp, false, checksum)
    {
        Q_ASSERT(functionCode() == QModbusPdu::Diagnostics);
    }

    ///
    /// \brief DiagnosticsResponse
    /// \param adu
    /// \param timestamp
    ///
    DiagnosticsResponse(const QModbusAdu& adu, const QDateTime& timestamp)
        : ModbusMessage(adu, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::Diagnostics);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && dataSize() > 2;
    }

    ///
    /// \brief subfunc
    /// \return
    ///
    quint16 subfunc() const {
        return makeWord(ModbusMessage::data(1), ModbusMessage::data(0), ByteOrder::LittleEndian);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return slice(2);
    }
};

#endif // DIAGNOSTICS_H
