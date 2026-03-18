#ifndef QMODBUSADUTCP_H
#define QMODBUSADUTCP_H

#include "qmodbusadu.h"
#include "numericutils.h"

///
/// \brief The QModbusAduTcp class
///
class QModbusAduTcp : public QModbusAdu
{
public:
    /// Minimum TCP frame size: header(6) + unitId(1) + function(1) = 8 bytes
    static constexpr int MinTcpFrameSize = 8;

    explicit QModbusAduTcp(const QByteArray& rawData)
        : QModbusAdu()
    {
        setRawData(rawData);
    }

    ///
    /// \brief isValid
    /// \return
    ///
    bool isValid() const override {
        if (_data.size() < MinTcpFrameSize)
            return false;

        return _pdu.isValid() && length() == _pdu.size() + 1;
    }

    ///
    /// \brief setRawData
    /// \param data
    ///
    void setRawData(const QByteArray& data) override {
        _data = data;
        if (_data.size() >= MinTcpFrameSize) {
            _pdu.setFunctionCode(QModbusPdu::FunctionCode(quint8(_data[7])));
            _pdu.setData(_data.mid(8, _data.size() - MinTcpFrameSize));
        } else {
            _pdu.setFunctionCode(QModbusPdu::Invalid);  // reset to invalid state
            _pdu.setData(QByteArray());
        }
    }

    ///
    /// \brief transactionId
    /// \return
    ///
    quint16 transactionId() const {
        if (_data.size() < MinTcpFrameSize)
            return 0;

        return makeUInt16(_data[1], _data[0], ByteOrder::Direct);
    }

    ///
    /// \brief setTransactionId
    /// \param id
    ///
    void setTransactionId(quint16 id) {
        if (_data.size() < MinTcpFrameSize)
            return;

        quint8 lo,hi;
        breakUInt16(id, lo, hi, ByteOrder::Direct);
        _data[1] = lo; _data[0] = hi;
    }

    ///
    /// \brief protocolId
    /// \return
    ///
    quint16 protocolId() const {
        if (_data.size() < MinTcpFrameSize)
            return 0;

        return makeUInt16(_data[3], _data[2], ByteOrder::Direct);
    }

    ///
    /// \brief length
    /// \return
    ///
    quint16 length() const {
        if (_data.size() < MinTcpFrameSize)
            return 0;

        return makeUInt16(_data[5], _data[4], ByteOrder::Direct);
    }

    ///
    /// \brief serverAddress
    /// \return
    ///
    quint8 serverAddress() const override {
        if (_data.size() < MinTcpFrameSize)
            return 0;

        return quint8(_data[6]);
    }

};

#endif // QMODBUSADUTCP_H
