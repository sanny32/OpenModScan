#ifndef MODBUSPDUINFO_H
#define MODBUSPDUINFO_H

#include <QDateTime>
#include <QModbusPdu>
#include "enums.h"
#include "modbusfunction.h"
#include "modbusexception.h"

///
/// \brief The ModbusPduInfo class
///
class ModbusPduInfo
{
public:
    explicit ModbusPduInfo() = default;
    explicit ModbusPduInfo(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, bool request){
        _timestamp = timestamp;
        _deviceId = deviceId;
        _request = request;
        _funcCode = pdu.isException() ? (pdu.functionCode() | QModbusPdu::ExceptionByte) : pdu.functionCode();
        _exceptionCode = pdu.exceptionCode();
        _data = pdu.data();
    }

    bool isValid() const {
        return !_data.isEmpty();
    }

    QDateTime timestamp() const {
        return _timestamp;
    }

    bool request() const {
        return _request;
    }

    bool deviceId() const {
        return _deviceId;
    }

    ModbusFunction function() const {
        return ModbusFunction((QModbusPdu::FunctionCode)_funcCode);
    }

    bool isException() const {
        return _funcCode & QModbusPdu::ExceptionByte;
    }

    ModbusException exception() const {
        return ModbusException((QModbusPdu::ExceptionCode)_exceptionCode);
    }

    QByteArray data() const {
        return _data;
    }

    quint16 startAddress() const {
        if(!_request) return 0;
        return _data[1];
    }

    quint16 length() const {
        if(!_request) return 0;
        return _data[3];
    }

    static QString formatByteValue(DataDisplayMode mode, uchar c) {
        switch(mode)
        {
            case DataDisplayMode::Decimal:
            case DataDisplayMode::Integer:
                return QString("%1").arg(QString::number(c), 3, '0');

            default:
                return QString("%1").arg(QString::number(c, 16).toUpper(), 2, '0');
        }
    }

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

    QString registerValues(DataDisplayMode mode) const {
        if(_request) return QString();

        QStringList values;
        for(auto i = 1; i < _data.size(); i++)
            values += ModbusPduInfo::formatByteValue(mode, _data[i]);

        return values.join(" ");
    }

private:
    QDateTime _timestamp;
    int _deviceId = -1;
    bool _request = false;
    quint8 _funcCode;
    quint8 _exceptionCode;
    QByteArray _data;
};
Q_DECLARE_METATYPE(ModbusPduInfo)

#endif // MODBUSPDUINFO_H
