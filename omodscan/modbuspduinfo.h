#ifndef MODBUSPDUINFO_H
#define MODBUSPDUINFO_H

#include <QDateTime>
#include <QModbusPdu>
#include "formatutils.h"
#include "modbusfunction.h"
#include "modbusexception.h"

///
/// \brief The ModbusPduInfo class
///
class ModbusPduInfo
{
public:
    ///
    /// \brief ModbusPduInfo
    ///
    explicit ModbusPduInfo() = default;

    ///
    /// \brief ModbusPduInfo
    /// \param pdu
    /// \param timestamp
    /// \param deviceId
    /// \param request
    ///
    explicit ModbusPduInfo(const QModbusPdu& pdu, const QDateTime& timestamp, int deviceId, bool request){
        _timestamp = timestamp;
        _deviceId = deviceId;
        _request = request;
        _funcCode = pdu.isException() ? (pdu.functionCode() | QModbusPdu::ExceptionByte) : pdu.functionCode();
        _exceptionCode = pdu.exceptionCode();
        _data = pdu.data();
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const {
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
    bool request() const {
        return _request;
    }

    ///
    /// \brief deviceId
    /// \return
    ///
    bool deviceId() const {
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
    /// \brief data
    /// \return
    ///
    QByteArray data() const {
        return _data;
    }

    ///
    /// \brief startAddress
    /// \return
    ///
    quint16 startAddress() const {
        if(!_request) return 0;
        return _data[1];
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        if(!_request) return 0;
        return _data[3];
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

    ///
    /// \brief registerValues
    /// \param mode
    /// \return
    ///
    QString registerValues(DataDisplayMode mode) const {
        if(_request) return QString();

        QStringList values;
        for(auto i = 1; i < _data.size(); i++)
            values += formatByteValue(mode, _data[i]);

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
