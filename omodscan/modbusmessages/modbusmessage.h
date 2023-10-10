#ifndef MODBUSMESSAGE_H
#define MODBUSMESSAGE_H

#include <QDateTime>
#include "qmodbusadu.h"
#include "modbuslimits.h"
#include "formatutils.h"
#include "modbusfunction.h"
#include "modbusexception.h"

///
/// \brief The ModbusMessage class
///
class ModbusMessage
{
public:
    enum Type{
        Adu,
        Pdu
    };

    ///
    /// \brief ModbusMessage
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param request
    /// \param checksum
    ///
    explicit ModbusMessage(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, bool request, int checksum)
        :_type(Type::Pdu)
        ,_protocol(protocol)
        ,_data(pdu.data())
        ,_funcCode(pdu.isException() ? (pdu.functionCode() | QModbusPdu::ExceptionByte) : pdu.functionCode())
        ,_exceptionCode(pdu.exceptionCode())
        ,_timestamp(timestamp)
        ,_transactionId(0)
        ,_protocolId(0)
        ,_length(0)
        ,_deviceId(deviceId)
        ,_request(request)
        ,_isValid(pdu.isValid())
        ,_checksum((protocol == QModbusAdu::Rtu) ? checksum : 0)
        ,_calcChecksum((protocol == QModbusAdu::Rtu) ? calcCRC(pdu, deviceId) : 0)
    {
    }

    ///
    /// \brief ModbusMessage
    /// \param adu
    /// \param timestamp
    /// \param request
    ///
    explicit ModbusMessage(const QModbusAdu& adu, const QDateTime& timestamp, bool request)
        :_type(Type::Adu)
        ,_protocol(adu.type())
        ,_data(adu.data())
        ,_funcCode(adu.isException() ? (adu.functionCode() | QModbusPdu::ExceptionByte) : adu.functionCode())
        ,_exceptionCode(adu.exceptionCode())
        ,_timestamp(timestamp)
        ,_transactionId(adu.transactionId())
        ,_protocolId(adu.protocolId())
        ,_length(adu.length())
        ,_deviceId(adu.serverAddress())
        ,_request(request)
        ,_isValid(adu.isValid())
        ,_checksum(adu.checksum())
        ,_calcChecksum(adu.calcChecksum())
    {
    }

    ///
    /// \brief ~ModbusMessage
    ///
    virtual ~ModbusMessage() = default;

    ///
    /// \brief create
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param request
    /// \return
    ///
    static const ModbusMessage* create(const QModbusPdu& pdu, QModbusAdu::Type protocol, int deviceId, const QDateTime& timestamp, bool request, int checksum = 0);

    ///
    /// \brief create
    /// \param adu
    /// \param timestamp
    /// \param request
    /// \return
    ///
    static const ModbusMessage* create(const QModbusAdu& adu, const QDateTime& timestamp, bool request);

    ///
    /// \brief parse
    /// \param data
    /// \param type
    /// \param protocol
    /// \param request
    /// \param checksum
    /// \return
    ///
    static const ModbusMessage* parse(const QByteArray& data, Type type, QModbusAdu::Type protocol, bool request, int checksum = 0);

    ///
    /// \brief type
    /// \return
    ///
    Type type() const {
        return _type;
    }

    ///
    /// \brief transactionId
    /// \return
    ///
    int transactionId() const {
        return _transactionId;
    }

    ///
    /// \brief protocolId
    /// \return
    ///
    int protocolId() const {
        return _protocolId;
    }

    ///
    /// \brief protocolType
    /// \return
    ///
    QModbusAdu::Type protocolType() const {
        return _protocol;
    }

    ///
    /// \brief length
    /// \return
    ///
    int length() const {
        return _length;
    }

    ///
    /// \brief isValid
    /// \return
    ///
    virtual bool isValid() const {
        return (_deviceId == 0 || ModbusLimits::slaveRange().contains(_deviceId)) &&
               matchingChecksum() && _isValid;
    }

    ///
    /// \brief timestamp
    /// \return
    ///
    QDateTime timestamp() const {
        return _timestamp;
    }

    ///
    /// \brief request
    /// \return
    ///
    bool isRequest() const {
        return _request;
    }

    ///
    /// \brief deviceId
    /// \return
    ///
    int deviceId() const {
        return _deviceId;
    }

    ///
    /// \brief function
    /// \return
    ///
    ModbusFunction function() const {
        return ModbusFunction((QModbusPdu::FunctionCode)_funcCode);
    }

    ///
    /// \brief functionCode
    /// \return
    ///
    QModbusPdu::FunctionCode functionCode() const {
        return (QModbusPdu::FunctionCode)(_funcCode & ~QModbusPdu::ExceptionByte);
    }

    ///
    /// \brief isException
    /// \return
    ///
    bool isException() const {
        return !isRequest() && (_funcCode & QModbusPdu::ExceptionByte);
    }

    ///
    /// \brief exception
    /// \return
    ///
    ModbusException exception() const {
        return ModbusException((QModbusPdu::ExceptionCode)_exceptionCode);
    }

    ///
    /// \brief toString
    /// \param mode
    /// \return
    ///
    QString toString(DataDisplayMode mode) const {
        return formatByteArray(mode, *this);
    }

    ///
    /// \brief rawData
    /// \return
    ///
    QByteArray rawData() const
    {
        return _data;
    }

    ///
    /// \brief checksum
    /// \return
    ///
    int checksum() const {
        return _checksum;
    }

    ///
    /// \brief calcChecksum
    /// \return
    ///
    int calcChecksum() const {
        return _calcChecksum;
    }

    ///
    /// \brief matchingChecksum
    /// \return
    ///
    bool matchingChecksum() const {
        return _calcChecksum == _checksum;
    }

    ///
    /// \brief operator QByteArray
    ///
    operator QByteArray() const {
        switch(_type)
        {
            case Adu:
                return _data;

            case Pdu:
            {
                QByteArray data;
                data.push_back(_deviceId);
                data.push_back(_funcCode);
                data.push_back(_data);
                return data;
            }
        }
        return QByteArray();
    }

protected:
    int dataSize() const {
        switch(_type)
        {
            case Adu:
                return qMax(0, _data.size() - 8);

            case Pdu:
                return _data.size();
        }
        return 0;
    }

    quint8 data(int idx) const {
        switch(_type)
        {
            case Adu:
                return idx < _data.size() - 8 ? _data.at(8 + idx) : 0;

            case Pdu:
                return idx < _data.size() ? _data.at(idx) : 0;
        }
        return 0;
    }

    QByteArray slice(int idx, int len = -1) const {
        switch(_type)
        {
            case Adu:
                return _data.mid(idx + 8, len);

            case Pdu:
                return _data.mid(idx, len);
        }
        return 0;
    }

private:
    int calcCRC(const QModbusPdu& pdu, int deviceId) {
        if(_protocol != QModbusAdu::Rtu)
            return 0;

        QByteArray data;
        data.push_back(deviceId);
        data.push_back(pdu.functionCode());
        data.push_back(pdu.data());

        return QModbusAdu::calculateCRC(data, data.length());
    }

private:
    const Type _type;
    QModbusAdu::Type _protocol;
    const QByteArray _data;
    const quint8 _funcCode;
    const quint8 _exceptionCode;
    const QDateTime _timestamp;
    const int _transactionId;
    const int _protocolId;
    const int _length;
    const int _deviceId;
    const bool _request;
    const bool _isValid;
    const int _checksum;
    const int _calcChecksum;
};
Q_DECLARE_METATYPE(const ModbusMessage*)

#endif // MODBUSMESSAGE_H
