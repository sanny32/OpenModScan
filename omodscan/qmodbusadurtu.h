#ifndef QMODBUSADURTU_H
#define QMODBUSADURTU_H

#include "qmodbusadu.h"
#include "numericutils.h"

///
/// \brief The QModbusAduRtu class
///
class QModbusAduRtu : public QModbusAdu
{
public:
    explicit QModbusAduRtu(const QByteArray& rawData)
        : QModbusAdu()
    {
        setRawData(rawData);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        return matchingChecksum() && _pdu.isValid();
    }

    ///
    /// \brief setRawData
    /// \param data
    ///
    void setRawData(const QByteArray& data) override {
        _data = data;
        _pdu.setFunctionCode(QModbusPdu::FunctionCode((quint8)_data[1]));
        _pdu.setData(_data.mid(2, _data.size() - 4));
    }

    ///
    /// \brief serverAddress
    /// \return
    ///
    quint8 serverAddress() const override {
        return quint8(_data[0]);
    }

    ///
    /// \brief checksum
    /// \return
    ///
    quint16 checksum() const {
        return makeWord(_data[_data.size() - 1], _data[_data.size() - 2], ByteOrder::LittleEndian);
    }

    ///
    /// \brief calcChecksum
    /// \return
    ///
    quint16 calcChecksum() const {
        const auto size = _data.size() - 2; // two bytes, CRC
        const auto data = _data.left(size);
        return calculateCRC(data, size);
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
};

#endif // QMODBUSADURTU_H
