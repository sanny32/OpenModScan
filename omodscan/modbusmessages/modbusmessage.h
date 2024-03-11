#ifndef MODBUSMESSAGE_H
#define MODBUSMESSAGE_H

#include <QDateTime>
#include "qmodbusadu.h"
#include "formatutils.h"
#include "modbusfunction.h"
#include "modbusexception.h"
#include "qmodbusadurtu.h"
#include "qmodbusadutcp.h"

///
/// \brief The ModbusMessage class
///
class ModbusMessage
{
public:
    enum ProtocolType{
        Rtu,
        Tcp
    };

    ///
    /// \brief ModbusMessage
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param request
    ///
    explicit ModbusMessage(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp, bool request)
        :_protocol(protocol)
        ,_request(request)
        ,_timestamp(timestamp)
    {
        const quint8 funcCode = pdu.isException() ? (pdu.functionCode() | QModbusPdu::ExceptionByte) : pdu.functionCode();
        switch(protocol)
        {
            case Rtu:
            {
                QByteArray data;
                QDataStream dataStream(&data, QIODevice::Append);
                dataStream << quint8(deviceId);
                dataStream << funcCode;
                dataStream.writeRawData(pdu.data(), pdu.dataSize());
                dataStream << QModbusAduRtu::calculateCRC(data, data.length());

                _adu = new QModbusAduRtu(data);
            }
            break;

            case Tcp:
            {
                QByteArray data;
                QDataStream dataStream(&data, QIODevice::Append);
                dataStream << quint16(0);
                dataStream << quint16(0);
                dataStream << quint16(pdu.size() + 1);
                dataStream << quint8(deviceId);
                dataStream << funcCode;
                dataStream.writeRawData(pdu.data(), pdu.dataSize());

                _adu = new QModbusAduTcp(data);
            }
            break;
        }
    }

    ///
    /// \brief ModbusMessage
    /// \param data
    /// \param protocol
    /// \param timestamp
    /// \param request
    ///
    explicit ModbusMessage(const QByteArray& data, ProtocolType protocol, const QDateTime& timestamp, bool request)
        :_protocol(protocol)
        ,_request(request)
        ,_timestamp(timestamp)
    {
        switch(protocol)
        {
            case Rtu:
                _adu = new QModbusAduRtu(data);
            break;

            case Tcp:
                _adu = new QModbusAduTcp(data);
            break;
        }
    }

    ///
    /// \brief ~ModbusMessage
    ///
    virtual ~ModbusMessage() {
        delete _adu;
    };

    ///
    /// \brief create
    /// \param pdu
    /// \param protocol
    /// \param deviceId
    /// \param timestamp
    /// \param request
    /// \return
    ///
    static const ModbusMessage* create(const QModbusPdu& pdu, ProtocolType protocol, int deviceId, const QDateTime& timestamp, bool request);

    ///
    /// \brief create
    /// \param data
    /// \param protocol
    /// \param timestamp
    /// \param request
    /// \return
    ///
    static const ModbusMessage* create(const QByteArray& data, ProtocolType protocol,  const QDateTime& timestamp, bool request);


    ///
    /// \brief protocolType
    /// \return
    ///
    ProtocolType protocolType() const {
        return _protocol;
    }

    ///
    /// \brief isValid
    /// \return
    ///
    virtual bool isValid() const {
        return _adu->isValid();
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
    /// \brief isException
    /// \return
    ///
    bool isException() const {
        return _adu->isException();
    }

    ///
    /// \brief deviceId
    /// \return
    ///
    int deviceId() const {
        return _adu->serverAddress();
    }

    ///
    /// \brief function
    /// \return
    ///
    ModbusFunction function() const {
        return ModbusFunction(_adu->functionCode());
    }

    ///
    /// \brief functionCode
    /// \return
    ///
    QModbusPdu::FunctionCode functionCode() const {
        return (QModbusPdu::FunctionCode)(_adu->functionCode() & ~QModbusPdu::ExceptionByte);
    }

    ///
    /// \brief exception
    /// \return
    ///
    ModbusException exception() const {
        return ModbusException(_adu->exceptionCode());
    }

    ///
    /// \brief deviceId
    /// \return
    ///
    const QModbusAdu* adu() const {
        return _adu;
    }

    ///
    /// \brief toString
    /// \param mode
    /// \return
    ///
    QString toString(DataDisplayMode mode) const {
        return formatUInt8Array(mode, *this);
    }

    ///
    /// \brief rawData
    /// \return
    ///
    QByteArray rawData() const
    {
        return _adu->rawData();
    }

    ///
    /// \brief operator QByteArray
    ///
    operator QByteArray() const {
        return _adu->rawData();
    }

protected:
    int dataSize() const {
        return _adu->pdu().dataSize();
    }

    quint8 at(int idx) const {
        const auto data =  _adu->pdu().data();
        return idx < data.size() ? data.at(idx) : 0;
    }

    QByteArray data(int idx, int len = -1) const {
        return _adu->pdu().data().mid(idx, len);
    }

private:
    QModbusAdu* _adu;
    ProtocolType _protocol;
    const bool _request;
    const QDateTime _timestamp;
};
Q_DECLARE_METATYPE(const ModbusMessage*)

#endif // MODBUSMESSAGE_H
