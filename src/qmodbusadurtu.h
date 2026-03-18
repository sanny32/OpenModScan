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
    /// Minimum RTU frame size: address(1) + function(1) + data(0+) + CRC(2)
    static constexpr int MinRtuFrameSize = 4;

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
        if (_data.size() < MinRtuFrameSize)
            return false;
        return matchingChecksum() && _pdu.isValid();
    }

    ///
    /// \brief setRawData
    /// \param data
    ///
    void setRawData(const QByteArray& data) override {
        _data = data;
        if (_data.size() >= MinRtuFrameSize) {
            _pdu.setFunctionCode(QModbusPdu::FunctionCode(quint8(_data[1])));
            _pdu.setData(_data.mid(2, _data.size() - MinRtuFrameSize));
        } else {
            _pdu.setFunctionCode(QModbusPdu::Invalid);  // reset to invalid state
            _pdu.setData(QByteArray());
        }
    }

    ///
    /// \brief serverAddress
    /// \return
    ///
    quint8 serverAddress() const override {
        if (_data.isEmpty())
            return 0;

        return quint8(_data[0]);
    }

    ///
    /// \brief checksum
    /// \return
    ///
    quint16 checksum() const {
        if (_data.size() >= MinRtuFrameSize) {
            return makeUInt16(_data[_data.size() - 1], _data[_data.size() - 2], ByteOrder::Direct);
        }
        return 0;
    }

    ///
    /// \brief calcChecksum
    /// \return
    ///
    quint16 calcChecksum() const {
        if (_data.size() < MinRtuFrameSize)
            return 0;

        const int crcDataSize = _data.size() - 2;
        const QByteArray crcData = _data.left(crcDataSize);
        return calculateCRC(crcData.constData(), crcDataSize);
    }

    ///
    /// \brief matchingChecksum
    /// \return
    ///
    bool matchingChecksum() const {
        return checksum() == calcChecksum();
    }

    inline static quint16 calculateCRC(const char* data, qint32 len) {
        if (len <= 0 || data == nullptr)
            return 0;

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
        crc = crc_reflect(crc, 16);
        return (crc >> 8) | (crc << 8);  // swap bytes
    }

private:
    inline static quint16 crc_reflect(quint16 data, qint32 len) {
        quint16 ret = data & 0x01;
        for (qint32 i = 1; i < len; i++) {
            data >>= 1;
            ret = (ret << 1) | (data & 0x01);
        }
        return ret;
    }
};

#endif // QMODBUSADURTU_H
