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
        return _deviceId >= 0 && !_data.isEmpty();
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
        return _funcCode & QModbusPdu::ExceptionByte;
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
        QByteArray rawData;
        rawData.push_back(_deviceId);
        rawData.push_back(_funcCode);
        rawData.push_back(_data);

        QStringList data;
        for(auto&& c : rawData)
                data += formatByteValue(mode, c);

        return data.join(" ");
    }

protected:
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
    {
    }

protected:
    const QByteArray _data;
    const quint8 _funcCode;
    const quint8 _exceptionCode;

private:
    const QDateTime _timestamp;
    const int _deviceId;
    const bool _request;
};

#endif // MODBUSMESSAGE_H
