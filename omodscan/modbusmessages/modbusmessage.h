#ifndef MODBUSMESSAGE_H
#define MODBUSMESSAGE_H

#include <QDateTime>
#include <QModbusPdu>
#include "formatutils.h"
#include "modbusfunction.h"
#include "modbusexception.h"

///
/// \brief The ModbusMessage class
///
class ModbusMessage
{
public:
    ///
    /// \brief ModbusMessage
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    /// \param request
    ///
    ModbusMessage(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, bool request)
        :_data(pdu.data())
        ,_funcCode(pdu.isException() ? (pdu.functionCode() | QModbusPdu::ExceptionByte) : pdu.functionCode())
        ,_exceptionCode(pdu.exceptionCode())
        ,_timestamp(timestamp)
        ,_deviceId(deviceId)
        ,_request(request)
        ,_isValid(pdu.isValid())
    {
    }

    ///
    /// \brief ~ModbusMessage
    ///
    virtual ~ModbusMessage() = default;

    ///
    /// \brief create
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    /// \param request
    /// \return
    ///
    static const ModbusMessage* create(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, bool request);

    ///
    /// \brief isValid
    /// \return
    ///
    virtual bool isValid() const {
        return _deviceId >= 0 && _isValid;
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
    /// \brief operator QByteArray
    ///
    operator QByteArray() const {
        QByteArray data;
        data.push_back(_deviceId);
        data.push_back(_funcCode);
        data.push_back(_data);
        return data;
    }

protected:
    const QByteArray _data;
    const quint8 _funcCode;
    const quint8 _exceptionCode;

private:
    const QDateTime _timestamp;
    const int _deviceId;
    const bool _request;
    const bool _isValid;
};
Q_DECLARE_METATYPE(const ModbusMessage*)

#endif // MODBUSMESSAGE_H
