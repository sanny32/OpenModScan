#ifndef QMODBUSADU_H
#define QMODBUSADU_H

#include <QModbusPdu>
#include <QByteArray>
#include "numericutils.h"

class QModbusAdu
{
public:
    enum Type {
        Rtu,
        Tcp
    };

    ///
    /// \brief QModbusAdu
    /// \param type
    /// \param data
    ///
    explicit QModbusAdu(Type type, const QByteArray& data)
        : _type(type)
    {
        setData(data);
    }

    ///
    /// \brief type
    /// \return
    ///
    Type type() const {
        return _type;
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const {
        return matchingChecksum() && _pdu.isValid() && length() == _pdu.size();
    }

    ///
    /// \brief data
    /// \return
    ///
    QByteArray data() const { return _data; }

    ///
    /// \brief setData
    /// \param data
    ///
    void setData(const QByteArray& data) {
        _data = data;

        _pdu.setFunctionCode(QModbusPdu::FunctionCode((quint8)_data[7]));
        if(_type == Rtu)
            _pdu.setData(_data.mid(8, _data.size() - 10));
        else
            _pdu.setData(_data.mid(8));
    }

    ///
    /// \brief transactionId
    /// \return
    ///
    quint16 transactionId() const {
        return makeWord(_data[1], _data[0], ByteOrder::LittleEndian);
    }

    ///
    /// \brief protocolId
    /// \return
    ///
    quint16 protocolId() const {
        return makeWord(_data[3], _data[2], ByteOrder::LittleEndian);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        return makeWord(_data[5], _data[4], ByteOrder::LittleEndian);
    }

    ///
    /// \brief serverAddress
    /// \return
    ///
    quint8 serverAddress() const {
        return quint8(_data[6]);
    }

    ///
    /// \brief functionCode
    /// \return
    ///
    QModbusPdu::FunctionCode functionCode() const {
        return _pdu.functionCode();
    }

    ///
    /// \brief exceptionCode
    /// \return
    ///
    QModbusPdu::ExceptionCode exceptionCode() const {
        return _pdu.exceptionCode();
    }

    ///
    /// \brief isException
    /// \return
    ///
    bool isException() const {
        return _pdu.isException();
    }

    ///
    /// \brief checksum
    /// \return
    ///
    quint16 checksum() const {
        return _type == Rtu ?
                   makeWord(_data[_data.size() - 1], _data[_data.size() - 2], ByteOrder::LittleEndian) :
                   quint16(0);
    }

    ///
    /// \brief calcChecksum
    /// \return
    ///
    quint16 calcChecksum() const {
        if(_type == Rtu)
        {
            const auto size = _data.size() - 2; // two bytes, CRC
            const auto data = _data.left(size);
            return QModbusAdu::calculateCRC(data, size);
        }
        return 0;
    }

    ///
    /// \brief matchingChecksum
    /// \return
    ///
    bool matchingChecksum() const {
        return checksum() == calcChecksum();
    }

    inline static quint16 calculateCRC(const char* data, qint32 len){
        quint16 crc = 0xFFFF;
        while (len--) {
            const quint8 c = *data++;
            for (qint32 i = 0x01; i & 0xFF; i <<= 1) {
                bool bit = crc & 0x8000;
                if (c & i)
                    bit = !bit;
                crc <<= 1;
                if (bit)
                    crc ^= 0x8005;
            }
            crc &= 0xFFFF;
        }
        crc = crc_reflect(crc & 0xFFFF, 16) ^ 0x0000;
        return (crc >> 8) | (crc << 8); // swap bytes
    }

private:
    inline static quint16 crc_reflect(quint16 data, qint32 len){
        quint16 ret = data & 0x01;
        for (qint32 i = 1; i < len; i++) {
            data >>= 1;
            ret = (ret << 1) | (data & 0x01);
        }
        return ret;
    }

private:
    Type _type;
    QByteArray _data;
    QModbusPdu _pdu;
};

#endif // QMODBUSADU_H
