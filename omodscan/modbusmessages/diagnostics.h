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
    /// \param transactionId
    /// \param timestamp
    ///
    DiagnosticsRequest(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        : ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, true)
    {
        Q_ASSERT(functionCode() == QModbusPdu::Diagnostics);
    }

    ///
    /// \brief DiagnosticsRequest
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    DiagnosticsRequest(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, true)
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
        return makeUInt16(ModbusMessage::at(1), ModbusMessage::at(0), ByteOrder::Direct);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return ModbusMessage::data(2);
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
    /// \param transactionId
    /// \param timestamp
    ///
    DiagnosticsResponse(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, int transactionId, const QDateTime& timestamp)
        :ModbusMessage(pdu, protocol, deviceId, transactionId, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::Diagnostics);
    }

    ///
    /// \brief DiagnosticsResponse
    /// \param data
    /// \param protocol
    /// \param timestamp
    ///
    DiagnosticsResponse(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp)
        : ModbusMessage(data, protocol, timestamp, false)
    {
        Q_ASSERT(functionCode() == QModbusPdu::Diagnostics);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return ModbusMessage::isValid() && (isException() ? true : (dataSize() > 2));
    }

    ///
    /// \brief subfunc
    /// \return
    ///
    quint16 subfunc() const {
        return makeUInt16(ModbusMessage::at(1), ModbusMessage::at(0), ByteOrder::Direct);
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return ModbusMessage::data(2);
    }
};

#endif // DIAGNOSTICS_H
